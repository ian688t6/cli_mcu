/*
* File  		:	cli.c
* Description	: 	cli cmd helper
* Author		: 	jia_yin@celfras.com
* Date			: 	2019.06.24	
*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

static cli_s gst_cli;

static int32_t cli_help(uint32_t *paui_args)
{
	return 0;
}

static cli_entry_s CLI_ENTRIES[] = {
	{"?", "Help", 0, cli_help},
	{0, 0, 0, 0},
};

static int32_t parse_entry_name(char *pc_cli_str, char *pc_name, char *pc_args)
{
	if (!pc_cli_str || !pc_name || !pc_args)
		return -1;

	/* Todo: Skip space */	
	while (' ' == *pc_cli_str)
		pc_cli_str ++;
	
	/* Todo: Get cmd name */
	while (0x00 != *pc_cli_str) {
		if (' ' == *pc_cli_str)
			break;
		*pc_name ++ = *pc_cli_str ++;	
	}
	*pc_name = '\0';	
	
	/* Todo: Skip space */	
	while (' ' == *pc_cli_str)
		pc_cli_str ++;

	/* Todo: Get cmd args */
	while (0x00 != *pc_cli_str) {
		if (' ' == *pc_cli_str)
			break;
		*pc_args ++ = *pc_cli_str ++;	
	}
	*pc_args = '\0';

	return 0;
}

static int32_t parse_args(uint32_t ui_args_cnt, char *pc_args, uint32_t *paui_args)
{
	int32_t i = 0;
	int32_t i_arg = 0;
	char ac_arg[CLI_ARG_SIZE] = {0};
	char *pc_arg = ac_arg;

	if (ui_args_cnt > CLI_ARG_MAXCOUNT)
		return -1;
	
	for (i = 0; i < ui_args_cnt; i ++) {
		memset(ac_arg, 0, sizeof(ac_arg));
		/* Todo: Skip space */	
		while (' ' == *pc_args)
			pc_args ++;

		/* Todo: Get cmd args */
		while (0x00 != *pc_args) {
			if (' ' == *pc_args)
				break;
			*pc_arg ++ = *pc_args ++;	
		}
		*pc_arg = '\0';
		i_arg = atoi(ac_arg);	
		paui_args[i] = i_arg;	
	}

	return 0;
}

static cli_entry_s *entry_lookup(char *pc_name)
{
	int32_t i = 0;
	cli_entry_s *pst_entry = 0;

	for (i = 0; 0 != CLI_ENTRIES[i].pc_name; i ++) {
		if (0 == strcmp(pc_name, CLI_ENTRIES[i].pc_name)) {
			pst_entry = &CLI_ENTRIES[i];	
			break;	
		}
	}
	
	return pst_entry;
}

static void cli_put_ch(uint8_t uc_ch)
{
	return;
}

static void cli_put_str(char *pc_str)
{
	return;
}

int32_t cli_recv(uint8_t uc_ch)
{
	int32_t i_ret = 0;
	uint8_t uc_rdch = uc_ch;

	if (gst_cli.uc_uart_ignor) {
		return 0;	
	}
	
    // backspace
	if (uc_rdch == 0x08) {
		if (gst_cli.uc_buf_pos > 0) {
			cli_put_ch(uc_rdch);
			cli_put_ch(0x20);
			cli_put_ch(uc_rdch);
			gst_cli.ac_cmd_buf[gst_cli.uc_buf_pos --] = 0;
		}
		return 0;	
	}

    // enter
	if (uc_rdch == 0x0D) {
		gst_cli.uc_uart_ready = 1;
		return 0;
	}
	
    // echo and collect
	if (gst_cli.uc_buf_pos + 1 >= CLI_BUFFER_SIZE) {
		gst_cli.uc_buf_pos = 0;
		memset(gst_cli.ac_cmd_buf, 0, sizeof(gst_cli.ac_cmd_buf));
	}
	gst_cli.ac_cmd_buf[gst_cli.uc_buf_pos ++] = uc_rdch;
	gst_cli.ac_cmd_buf[gst_cli.uc_buf_pos] = 0;	
	cli_put_ch(uc_rdch);

	return i_ret;
}

void cli_init(void)
{
	memset(&gst_cli, 0x0, sizeof(cli_s));	
	/* Todo: 1. uart_init */	
	/* Todo: 2. set uart recv cb */	
	return;
}

void cli_run(void)
{
	int32_t i_ret = 0;	
	cli_entry_s *pst_entry = 0;
	char ac_cmd_buf[CLI_CMD_SIZE] 		= {0};
	char ac_arg_buf[CLI_ARG_SIZE] 		= {0};
	uint32_t aui_args[CLI_ARG_MAXCOUNT] = {0};	
	
	for (;;) {
		if (gst_cli.uc_uart_ready) {
			gst_cli.uc_uart_ignor = 1;
			cli_put_str(CRLF);
			if (gst_cli.uc_buf_pos < 1) {
				goto endnrm2;	
			}

			i_ret = parse_entry_name(ac_cmd_buf, ac_cmd_buf, ac_arg_buf);
			if (0 != i_ret) {
				goto endnrm;	
			}
			
			pst_entry = entry_lookup(ac_cmd_buf);
			if (pst_entry) {
				parse_args(pst_entry->ui_args_cnt, ac_arg_buf, aui_args);					
				if (!pst_entry->pf_cb(aui_args)) {
                    goto endnrm;
				}	
			}
			endnrm:
			gst_cli.uc_buf_pos = 0;
			memset((char *) gst_cli.ac_cmd_buf, 0, sizeof(gst_cli.ac_cmd_buf));
			endnrm2:
        	cli_put_str("#");
			gst_cli.uc_uart_ready = 0;
			gst_cli.uc_uart_ignor = 0;
		}
	}

	return;
}

