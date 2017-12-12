#include "test_common.cpp"

int main() {
#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(216);
#endif

#define  MAX_LEN 200 * 1024
	char* kcpmeg = new char[MAX_LEN];
	for (size_t i = 0; i < MAX_LEN; i+=10)
	{
		sprintf(kcpmeg+i, "tick-%05d", i);
	}


	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_conv_t conv = kcpuv_connect(kcpuv, "192.168.56.128", 9527);
	uint64_t t = get_tick_ms();
	uint64_t nextSend = t + 1000;
	while (get_tick_ms() - t < 15000000) {
		kcpuv_run(kcpuv);

		kcpuv_msg_t msg;
		while (true) {
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			char buf[MAX_LEN] = { 0 };
			strncpy(buf, (const char*)msg.data, msg.size);
			//printf("conv: %d recv: %s\n", msg.conv, buf);
			kcpuv_msg_free(&msg);
		}

		uint64_t cur = get_tick_ms();
		//if (cur > nextSend) {
			char xxx[MAX_LEN];
#if defined(PLATFORM_WINDOWS)
			sprintf(xxx, "tick - %I64d", cur);
#else
			sprintf(xxx, "tick - %"PRIu64, cur);
#endif
			//kcpuv_send(kcpuv, conv, xxx, strlen(xxx));
			kcpuv_send(kcpuv, conv, kcpmeg, strlen(kcpmeg));
			//printf("11111111111111\n");
			nextSend = cur + 1000;
		//}
		sleep_ms(1);
	}
	kcpuv_destroy(kcpuv);

	delete[] kcpmeg;
	return 0;
}
