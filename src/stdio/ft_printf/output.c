/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmakino <hmakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/03 04:29:38 by hmakino           #+#    #+#             */
/*   Updated: 2023/02/06 01:46:43 by hiroaki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static size_t	buffering(char *str, t_info *info)
{
	int fd;
	size_t	len;

	len = 0;
	fd = STDOUT_FILENO;
	while (!info->flag_align && !info->flag_zero && 0 < info->width--)
		len += ft_putchar_fd(' ', fd);
	if (!info->spec_cs)
	{
		if (info->sign)
			len += ft_putchar_fd(info->sign, fd);
		else if (info->flag_space)
			len += ft_putchar_fd(' ', fd);
	}
	while ((info->has_prec < 0 && 0 < (int)info->prec--) || \
		(!info->flag_align && info->flag_zero && 0 < info->width--))
		len += ft_putchar_fd('0', fd);
	while (0 < info->len--)
		len += ft_putchar_fd(*str++, fd);
	if (info->flag_align)
	{
		while (0 < info->width--)
		{
			if (info->flag_zero)
				len += ft_putchar_fd('0', fd);
			else
				len += ft_putchar_fd(' ', fd);
		}
	}
	return (len);
}

static char	*spec_pct_c(va_list ap, t_info *info)
{
	char	*str;

	str = ft_calloc(2, sizeof(char));
	if (str == NULL)
		return (NULL);
	if (info->spec == 'c')
		str[0] = va_arg(ap, int);
	else
		str[0] = '%';
	info->len = 1;
	if (info->width > 0)
		info->width -= (int)info->len;
	return (str);
}

static char	*spec_s(va_list ap, t_info *info)
{
	char	*str;

	str = va_arg(ap, char *);
	if (str == NULL)
		str = (char *)"(null)";
	if (!(info->has_prec && (!info->digit_prec || !info->prec)))
		info->len = ft_strlen((char *)str);
	if (info->prec > 0 && info->prec < info->len)
		info->len = info->prec;
	if (info->width > 0)
		info->width -= (int)info->len;
	return (ft_strdup(str));
}

static char	*spec_i_d_u_p_x_X(va_list ap, t_info *info)
{
	char	*str;

	if (ft_strchr("id", info->spec))
		str = printf_itoa_base(is_neg(va_arg(ap, int), info), info);
	if (ft_strchr("uxX", info->spec))
		str = printf_itoa_base(va_arg(ap, unsigned int), info);
	if (info->spec == 'p')
		str = printf_itoa_base(va_arg(ap, unsigned long), info);
	if (info->flag_zero && (info->has_prec || info->flag_align))
		info->flag_zero = 0;
	if (*str == '0' && info->has_prec && (!info->digit_prec || !info->prec))
		info->len = 0;
	if (info->width > 0)
	{
		if (info->prec && info->len < info->prec)
			info->width -= ((int)info->prec + (0 < info->sign) + info->flag_space);
		else
			info->width -= (info->len + (0 < info->sign) + info->flag_space);
	}
	if (info->len < info->prec)
	{
		info->has_prec = -1;
		info->prec -= info->len;
	}
	return (str);
}

size_t 	process_spec(va_list ap, t_info *info)
{
	size_t	len;
	char	*str;

	if (ft_strchr("%c", info->spec))
		str = spec_pct_c(ap, info);
	else if (info->spec == 's')
		str = spec_s(ap, info);
	else if (ft_strchr("idupxX", info->spec))
		str = spec_i_d_u_p_x_X(ap, info);
	else
		return (0);
	len = buffering(str, info);
	ft_free(str);
	return (len);
}