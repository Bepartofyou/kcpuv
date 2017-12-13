#include "test_common.cpp"

static uint64_t ggTime = 0;
static uint64_t ggCount = 0;
static uint64_t ggCount_last = 0;



void check() {
	if (ggTime == 0) {
		ggTime = get_tick_ms();
	}
	else {
		if (get_tick_ms() - ggTime > 1000) {
			ggTime = get_tick_ms();

			//printf("data kBps: %d\n", (ggCount - ggCount_last) / 1000);
			ggCount_last = ggCount;
		}
	}
}

int main() {
#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(216);
#endif

#define  MAX_LEN 10 * 1024
	char* kcpmeg = new char[MAX_LEN];
	for (size_t i = 0; i < MAX_LEN; i+=10)
	{
		sprintf(kcpmeg+i, "tick-%05d", i);
	}

	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_conv_t conv = kcpuv_connect(kcpuv, "192.168.56.128", 9527);
	uint64_t t = get_tick_ms();
	//uint64_t nextSend = t + 1000;
	uint64_t nextSend = t;
	while (get_tick_ms() - t < 15000000) {
		//kcpuv_run(kcpuv);

		uint64_t cur = get_tick_ms();
		ggCount += strlen(kcpmeg);
		kcpuv_send(kcpuv, conv, kcpmeg, strlen(kcpmeg));
		nextSend = get_tick_ms();

		check();
		
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
	}
	kcpuv_destroy(kcpuv);

	delete[] kcpmeg;
	return 0;
}
