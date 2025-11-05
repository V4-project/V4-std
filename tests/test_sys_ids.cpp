/**
 * @file test_sys_ids.cpp
 * @brief Tests for generated SYS ID definitions
 */

#include "v4std/sys_ids.h"
#include <cassert>
#include <cstdio>

int main()
{
  // Test: LED operations range (0x0100-0x01FF)
  {
    assert(V4SYS_LED_ON == 0x0100);
    assert(V4SYS_LED_OFF == 0x0101);
    assert(V4SYS_LED_TOGGLE == 0x0102);
    assert(V4SYS_LED_SET == 0x0103);
    assert(V4SYS_LED_GET == 0x0110);
    printf("✓ LED SYS IDs in correct range (0x0100-0x01FF)\n");
  }

  // Test: BUTTON operations range (0x0200-0x02FF)
  {
    assert(V4SYS_BUTTON_READ == 0x0200);
    assert(V4SYS_BUTTON_WAIT == 0x0201);
    printf("✓ BUTTON SYS IDs in correct range (0x0200-0x02FF)\n");
  }

  // Test: TIMER operations range (0x0300-0x03FF)
  {
    assert(V4SYS_TIMER_START == 0x0300);
    assert(V4SYS_TIMER_STOP == 0x0301);
    assert(V4SYS_TIMER_ONESHOT == 0x0302);
    assert(V4SYS_TIMER_RUNNING == 0x0310);
    printf("✓ TIMER SYS IDs in correct range (0x0300-0x03FF)\n");
  }

  // Test: UART operations range (0x0400-0x04FF)
  {
    assert(V4SYS_UART_READ == 0x0400);
    assert(V4SYS_UART_WRITE == 0x0401);
    assert(V4SYS_UART_AVAILABLE == 0x0402);
    printf("✓ UART SYS IDs in correct range (0x0400-0x04FF)\n");
  }

  // Test: I2C operations range (0x0500-0x05FF)
  {
    assert(V4SYS_I2C_READ_REG == 0x0500);
    assert(V4SYS_I2C_WRITE_REG == 0x0501);
    printf("✓ I2C SYS IDs in correct range (0x0500-0x05FF)\n");
  }

  // Test: SPI operations range (0x0600-0x06FF)
  {
    assert(V4SYS_SPI_TRANSFER == 0x0600);
    printf("✓ SPI SYS IDs in correct range (0x0600-0x06FF)\n");
  }

  // Test: ADC operations range (0x0700-0x07FF)
  {
    assert(V4SYS_ADC_READ == 0x0700);
    printf("✓ ADC SYS IDs in correct range (0x0700-0x07FF)\n");
  }

  // Test: PWM operations range (0x0800-0x08FF)
  {
    assert(V4SYS_PWM_SET == 0x0800);
    assert(V4SYS_PWM_START == 0x0801);
    assert(V4SYS_PWM_STOP == 0x0802);
    printf("✓ PWM SYS IDs in correct range (0x0800-0x08FF)\n");
  }

  // Test: STORAGE operations range (0x0900-0x09FF)
  {
    assert(V4SYS_STORAGE_READ == 0x0900);
    assert(V4SYS_STORAGE_WRITE == 0x0901);
    printf("✓ STORAGE SYS IDs in correct range (0x0900-0x09FF)\n");
  }

  // Test: DISPLAY operations range (0x0A00-0x0AFF)
  {
    assert(V4SYS_DISPLAY_PUTC == 0x0A00);
    assert(V4SYS_DISPLAY_CLEAR == 0x0A01);
    printf("✓ DISPLAY SYS IDs in correct range (0x0A00-0x0AFF)\n");
  }

  // Test: RNG operations range (0x0B00-0x0BFF)
  {
    assert(V4SYS_RNG_READ == 0x0B00);
    printf("✓ RNG SYS IDs in correct range (0x0B00-0x0BFF)\n");
  }

  // Test: System/Capability operations range (0x0F00-0x0FFF)
  {
    assert(V4SYS_CAP_COUNT == 0x0F00);
    assert(V4SYS_CAP_EXISTS == 0x0F01);
    assert(V4SYS_CAP_FLAGS == 0x0F02);
    assert(V4SYS_CAP_HANDLE == 0x0F03);
    assert(V4SYS_SYS_VERSION == 0x0FF0);
    assert(V4SYS_SYS_PLATFORM == 0x0FF1);
    printf("✓ System/Capability SYS IDs in correct range (0x0F00-0x0FFF)\n");
  }

  // Test: All IDs are in V4-std range (0x0100-0x0FFF)
  {
    // LED
    assert(V4SYS_LED_ON >= 0x0100 && V4SYS_LED_ON <= 0x0FFF);
    assert(V4SYS_LED_GET >= 0x0100 && V4SYS_LED_GET <= 0x0FFF);

    // BUTTON
    assert(V4SYS_BUTTON_READ >= 0x0100 && V4SYS_BUTTON_READ <= 0x0FFF);

    // TIMER
    assert(V4SYS_TIMER_START >= 0x0100 && V4SYS_TIMER_START <= 0x0FFF);

    // System
    assert(V4SYS_SYS_PLATFORM >= 0x0100 && V4SYS_SYS_PLATFORM <= 0x0FFF);

    printf("✓ All SYS IDs within V4-std range (0x0100-0x0FFF)\n");
  }

  // Test: No overlapping IDs between categories
  {
    // Verify each category is in its designated range
    assert((V4SYS_LED_ON & 0xFF00) == 0x0100);
    assert((V4SYS_BUTTON_READ & 0xFF00) == 0x0200);
    assert((V4SYS_TIMER_START & 0xFF00) == 0x0300);
    assert((V4SYS_UART_READ & 0xFF00) == 0x0400);
    assert((V4SYS_I2C_READ_REG & 0xFF00) == 0x0500);
    assert((V4SYS_SPI_TRANSFER & 0xFF00) == 0x0600);
    assert((V4SYS_ADC_READ & 0xFF00) == 0x0700);
    assert((V4SYS_PWM_SET & 0xFF00) == 0x0800);
    assert((V4SYS_STORAGE_READ & 0xFF00) == 0x0900);
    assert((V4SYS_DISPLAY_PUTC & 0xFF00) == 0x0A00);
    assert((V4SYS_RNG_READ & 0xFF00) == 0x0B00);
    assert((V4SYS_CAP_COUNT & 0xFF00) == 0x0F00);

    printf("✓ No overlapping between SYS ID categories\n");
  }

  printf("\n✅ All SYS ID tests passed!\n");
  return 0;
}
