.text

.weak exception_handler
.weak timer_handler
.weak ext_irq_handler
.weak wdt_handler
.weak cfs_handler
.weak uart0_rx_handler
.weak uart0_tx_handler
.weak uart1_rx_handler
.weak uart1_tx_handler
.weak spi_handler
.weak twi_handler
.weak xirq_handler
.weak neoled_handler
.weak dma_handler
.weak sdi_handler
.weak gptmr_handler
.weak onewire_handler
.weak slink_handler
.weak trng_handler

.align 16
.global _vector_table
_vector_table:
    .org  _vector_table + 0*4
    jal zero, exception_handler
    
    .org  _vector_table + 7*4
    jal zero, timer_handler
    
    .org  _vector_table + 11*4
    jal zero, ext_irq_handler

    .org  _vector_table + 16*4
    jal zero, wdt_handler
    jal zero, cfs_handler
    jal zero, uart0_rx_handler
    jal zero, uart0_tx_handler
    jal zero, uart1_rx_handler
    jal zero, uart1_tx_handler
    jal zero, spi_handler
    jal zero, twi_handler
    jal zero, xirq_handler
    jal zero, neoled_handler
    jal zero, dma_handler
    jal zero, sdi_handler
    jal zero, gptmr_handler
    jal zero, onewire_handler
    jal zero, slink_handler
    jal zero, trng_handler

.global default_handler
default_handler:
exception_handler:
timer_handler:
ext_irq_handler:
wdt_handler:
cfs_handler:
uart0_rx_handler:
uart0_tx_handler:
uart1_rx_handler:
uart1_tx_handler:
spi_handler:
twi_handler:
xirq_handler:
neoled_handler:
dma_handler:
sdi_handler:
gptmr_handler:
onewire_handler:
slink_handler:
trng_handler:
    nop
    mret
