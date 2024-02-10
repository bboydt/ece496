#pragma once

// @todo add more levels
#define VCCIO_3V3 6500

#define VCCIO0_BASE ((uintptr_t)(0x00000000))
#define VCCIO0_CTRL (*(uint32_t*)(VCCIO_BASE + 0x00))
#define VCCIO0_CH0  (*(uint32_t*)(VCCIO_BASE + 0x04))
#define VCCIO0_CH1  (*(uint32_t*)(VCCIO_BASE + 0x08))
#define VCCIO0_CH2  (*(uint32_t*)(VCCIO_BASE + 0x0C))

