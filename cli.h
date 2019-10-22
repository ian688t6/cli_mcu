#ifndef __CLI_H__
#define __CLI_H__

#define CRLF	"\r\n"
#define CLI_BUFFER_SIZE		(129)
#define CLI_CMD_SIZE		(32)
#define CLI_ARG_SIZE		(96)
#define CLI_ARG_MAXCOUNT	(5)

typedef struct {
	char 		ac_cmd_buf[CLI_BUFFER_SIZE];
	uint8_t		uc_buf_pos;
	uint8_t		uc_uart_ready;
	uint8_t		uc_uart_ignor;
} cli_s;

typedef int32_t (*cli_cb)(uint32_t *paui_args);

typedef struct {
	char 			*pc_name;
	char 			*pc_usage;
	uint32_t 		ui_args_cnt;
	cli_cb			pf_cb;	
} cli_entry_s;

extern void cli_init(void);

extern void cli_run(void);

#endif

