#include <stdlib.h>
//#include <string.h>
//#include <inttypes.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 3333333UL
#endif

#include "CWire.h"

#ifndef TWI0
#define TWI0
#endif

#include "twi_pins.h"

volatile u8 sleepStack = 0;

void pauseDeepSleep(u8 moduelAddr);
void restoreSleep(u8 moduelAddr);

TwoWire* twi0_wire;

#define TWI_BAUD(freq, t_rise) ((F_CPU / freq) / 2) - (5 + (((F_CPU / 1000000) * t_rise) / 2000))
u8 MasterCalcBaud(u32 freq) {
    i16 baud;
    if (freq >= 600000) {          // assuming 1.5kOhm
        baud = TWI_BAUD(freq, 250);
    } else if (freq >= 400000) {   // assuming 2.2kOhm
        baud = TWI_BAUD(freq, 400);
    } else {                            // assuming 4.7kOhm
        baud = TWI_BAUD(freq, 600);
    }
    
    const u8 baudlimit = 0;
    
    if (baud < baudlimit) {
        return baudlimit;
    } else if (baud > 255) {
        return 255;
    }

    return (u8)baud;
}

void TwoWire_init(TwoWire *self, TWI_t *twi_module) {
    self->_module = twi_module;
    twi0_wire = self;
}

u8 TwoWire_pins(TwoWire *self, u8 sda, u8 scl) {
    return TWI0_Pins(sda, scl);
}

void TwoWire_usePullups(TwoWire *self) {
    TWI0_usePullups();
}

u8 TwoWire_setClock(TwoWire *self, u32 freq) {
    TWI_t* module = self->_module;
    if (__builtin_constant_p(freq)) {
        if ((freq < 1000) || (freq > 15000000)) {
            return 1;
        }
    } else {
        if (freq < 1000) {
            return 1;
        }
    }
    if (self->_bools._hostEnabled == 1) {
        u8 newBaud = MasterCalcBaud(freq);
        u8 oldBaud = module->MCTRLA;
        if (newBaud != oldBaud) {
            u8 restore = module->MCTRLA;
            module->MCTRLA = 0;
            module->MBAUD = newBaud;
            if (freq > 400000) {
                module->CTRLA |= TWI_FMPEN_bm;
            } else {
                module->CTRLA &= ~TWI_FMPEN_bm;
            }
            module->MCTRLA = restore;
            if (restore & TWI_ENABLE_bm) {
                module->MSTATUS = TWI_BUSSTATE_IDLE_gc;
            }
        }
        return 0;
    }
    return 1;
}

//Master
void TwoWire_Master_begin(TwoWire *self) {
    TWI0_ClearPins();
    
    self->_bools._hostEnabled = 1;
    TWI_t* module = self->_module;
    module->MCTRLA = TWI_ENABLE_bm;
    module->MSTATUS = TWI_BUSSTATE_IDLE_gc;
    
    TwoWire_setClock(self, DEFAULT_FREQUENCY);
}

void TwoWire_endMaster(TwoWire *self) {
    if (true == self->_bools._hostEnabled) {
        self->_module->MCTRLA = 0x00;
        self->_module->MBAUD  = 0x00;
        self->_bools._hostEnabled  = 0x00;
    }
}

void TwoWire_beginTransmission(TwoWire *self, u8 addr) {
    if (__builtin_constant_p(addr) > 0x7F) {
        badArg("Supplied address seems to be 8 bit. Only 7 bit addresses are supported");
        return;
    }
    if (self->_bools._hostEnabled) {
        self->_clientAddress = addr << 1;
        self->_bytesToReadWrite = 0;
        self->_bytesReadWritten = 0;
    }
}
u8 TwoWire_endTransmission(TwoWire *self, u8 sendStop) {
    return TwoWire_masterTransmit(self, &self->_bytesToReadWrite, self->_hostBuffer, self->_clientAddress, sendStop);
}

twi_buf_index_t TwoWire_requestFrom(TwoWire *self, u8 addr, twi_buf_index_t qty, u8 sendStop) {
    if (__builtin_constant_p(qty)) {
        if (qty > TWI_BUFFER_LENGTH) {
            badArg("requestFrom requests more bytes than buffer space");
        }
    }
    if (qty >= TWI_BUFFER_LENGTH) {
        qty = TWI_BUFFER_LENGTH;
    }
    
    self->_clientAddress = addr << 1;
    self->_bytesToReadWrite = qty;
    self->_bytesReadWritten = 0;
    
    return TwoWire_masterReceive(self, &self->_bytesToReadWrite, self->_hostBuffer, self->_clientAddress, sendStop);
}

u8 TwoWire_masterTransmit(TwoWire *self, twi_buf_index_t *length, u8 *buff, u8 addr, u8 sendStop) {
    TWI_t* module = self->_module;
    __asm__ __volatile__("\n\t" : "+z"(module));
    
    TWI_INIT_ERROR;
    u8 currentSM;
    u8 currentStatus;
    u8 stat = 0;
    
    u16 dataToWrite = *length;
    #if defined (TWI_TIMEOUT_ENABLE)
    u16 timeout = (F_CPU/1000);
    #endif

    if ((module->MCTRLA & TWI_ENABLE_bm) == 0x00) {
        return TWI_ERR_UNINIT;
    }
    
    while (1) {
        currentStatus = module->MSTATUS;
        currentSM = currentStatus & TWI_BUSSTATE_gm;
        
        if (currentSM == TWI_BUSSTATE_UNKNOWN_gc) {
            return TWI_ERR_UNINIT;
        }
        
        #if defined (TWI_TIMEOUT_ENABLE)
        if (--timeout == 0) {
            if (currentSM == TWI_BUSSTATE_OWNER_gc) {
                TWI_SET_ERROR(TWI_ERR_TIMEOUT);
            } else if (currentSM == TWI_BUSSTATE_IDLE_gc) {
                TWI_SET_ERROR(TWI_ERR_PULLUP);
            } else {
                TWI_SET_ERROR(TWI_ERR_UNDEFINED);
            }
            break;
        }
        #endif

        if (currentStatus & TWI_ARBLOST_bm) {
            TWI_SET_ERROR(TWI_ERR_BUS_ARB);
            break;
        }
        
        if (currentSM != TWI_BUSSTATE_BUSY_gc) {
            if (stat == 0x00) {
                module->MADDR = ADD_WRITE_BIT(addr);
                stat |= 0x01;
                #if defined (TWI_TIMEOUT_ENABLE)
                timeout = (F_CPU/1000);
                #endif
            } else {
                if (currentStatus & TWI_WIF_bm) {
                    if (currentStatus & TWI_RXACK_bm) {
                        if (stat & 0x02) {
                            if (dataToWrite != 0) {
                                TWI_SET_ERROR(TWI_ERR_ACK_DAT);
                            } 
                        } else {
                            TWI_SET_ERROR(TWI_ERR_ACK_ADR);
                        }
                        break;
                    } else {
                        if (dataToWrite != 0) {
                            module->MDATA = *buff;
                            buff++;
                            dataToWrite--;
                            stat |= 0x02;
                            #if defined (TWI_TIMEOUT_ENABLE) 
                            timeout = (F_CPU/1000);
                            #endif
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
    
    *length -= dataToWrite;
    if ((sendStop != 0) || (TWI_ERR_SUCCESS != TWI_GET_ERROR)) {
        module->MCTRLB = TWI_MCMD_STOP_gc;
    }
    return TWI_GET_ERROR;
}

u8 TwoWire_masterReceive(TwoWire *self, u16 *length, u8 *buff, u8 addr, u8 sendStop) {
    TWI_t *module = self->_module;
    __asm__ __volatile__("\n\t" : "+z"(module));
    
    TWIR_INIT_ERROR;
    u16 dataToRead = *length;
    
    u8 currentSM;
    u8 currentStatus;
    u8 state = 0;
    #if defined (TWI_TIMEOUT_ENABLE)
    u16 timeout = (F_CPU/1000);
    #endif

    while(1) {
        currentStatus = module->MSTATUS;
        currentSM = currentStatus & TWI_BUSSTATE_gm;
        
        if (currentSM == TWI_BUSSTATE_UNKNOWN_gc) {
            TWIR_SET_ERROR(TWI_ERR_UNINIT);
            break;
        }
        
        #if defined (TWI_TIMEOUT_ENABLE)
        if (--timeout == 0) {
            if (currentSM == TWI_BUSSTATE_OWNER_gc) {
                TWIR_SET_ERROR(TWI_ERR_TIMEOUT);
            } else if (currentSM == TWI_BUSSTATE_IDLE_gc) {
                TWIR_SET_ERROR(TWI_ERR_PULLUP);
            } else {
                TWIR_SET_ERROR(TWI_ERR_UNDEFINED);
            }
            break;
        }
        #endif

        if (currentStatus & TWI_ARBLOST_bm) {
            TWIR_SET_ERROR(TWI_ERR_BUS_ARB);
            break;
        }
        
        if (currentSM != TWI_BUSSTATE_BUSY_gc) {
            if (state == 0x00) {
                module->MADDR = ADD_READ_BIT(addr);
                state |= 0x01;
                #if defined (TWI_TIMEOUT_ENABLE)
                timeout = (F_CPU / 1000);
                #endif
            } else {
                if (currentStatus & TWI_WIF_bm) {
                    TWIR_SET_ERROR(TWI_ERR_ACK_ADR);
                    module->MCTRLB = TWI_MCMD_STOP_gc;
                    break;
                } else if (currentStatus & TWI_RIF_bm) {
                    *buff = module->MDATA;
                    buff++;
                    #if defined (TWI_TIMEOUT_ENABLE)
                    timeout = (F_CPU / 1000);
                    #endif
                    if (--dataToRead != 0) {
                        module->MCTRLB = TWI_MCMD_RECVTRANS_gc;
                    } else {
                        if (sendStop != 0) {
                            module->MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
                        } else {
                            module->MCTRLB = TWI_ACKACT_bm;
                        }
                        break;
                    }
                }
            }
        }
    }
    *length -= dataToRead;
    return TWIR_GET_ERROR;
}

// Slave
void TwoWire_Slave_begin(TwoWire *self, u8 addr, u8 bc, u8 sec_addr) {
    if (__builtin_constant_p(addr)) {
        if (addr > 0x7F) {
            badArg("TWI addresses must be supplied in 7-bit format");
            return;
        }
    }
    
    #if defined(TWI_MANDS)
    if (self->_bools._clientEnabled == 1) {
        return;
    }
    #else
    if ((self->_bools._hostEnabled | self->_bools._clientEnabled) == 1) {
        return;
    }
    #endif

    TWI0_ClearPins();
    
    self->_bools._clientEnabled = 1;
    self->client_irq_mask= TWI_COLL_bm;
    TWI_t* module = self->_module;
    module->SADDR = (addr << 1) | bc; //broadcast
    module->SADDRMASK = sec_addr;
    module->SCTRLA = TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm | TWI_ENABLE_bm;
}

void TwoWire_endSlave(TwoWire *self) {
    if (self->_bools._clientEnabled == 1) {
        self->_module->SADDR = 0x00;
        self->_module->SCTRLA = 0x00;
        self->_module->SADDRMASK = 0x00;
        self->_bools._clientEnabled = 0x00;
    }
}

u8 TwoWire_getIncomingAddress(TwoWire *self) {
    #if defined (TWI_MANDS) 
    return self->_incomingAddress;
    #else
    return self->_clientAddress;
    #endif
}

twi_buf_index_t TwoWire_getBytesRead(TwoWire *self) {
    twi_buf_index_t num = self->_bytesTransmittedS;
    self->_bytesTransmittedS = 0;
    return num;
}

u8 TwoWire_slaveTransactionOpen(TwoWire *self) {
    u8 status = self->_module->SSTATUS;
    if (!(status & TWI_AP_bm)) {
        return 0;
    }
    if (status & TWI_DIR_bm) {
        return 2;
    }
    return 1;
}

u8 TwoWire_checkPinLevels(TwoWire *self) {
    return TWI0_checkPinLevel();
}

void TwoWire_onReceive(TwoWire *self, void (*function)(u8)) {
    self->user_onReceive = function;
}

void TwoWire_onRequest(TwoWire *self, void (*function)(void)) {
    self->user_onRequest = function;
}

ISR(TWI0_TWIS_vect) {
  TwoWire_HandleSlaveIRQ(twi0_wire);
}

void TwoWire_HandleSlaveIRQ(TwoWire *wire_s) {
  if (wire_s == NULL) {
    return;
  }

  u8 *address,  *buffer;
  twi_buf_index_t *head, *tail;
  #if defined(TWI_MANDS)
    address = &(wire_s->_incomingAddress);
    head    = &(wire_s->_bytesToReadWriteS);
    tail    = &(wire_s->_bytesReadWrittenS);
    buffer  =   wire_s->_clientBuffer;
  #else
    address = &(wire_s->_clientAddress);
    head    = &(wire_s->_bytesToReadWrite);
    tail    = &(wire_s->_bytesReadWritten);
    buffer  =   wire_s->_hostBuffer;
  #endif

  #if defined(TWI_MANDS)
    wire_s->_bools._toggleStreamFn = 0x01;
  #endif

  u8 action = 0;
  u8 clientStatus = wire_s->_module->SSTATUS;


  if (clientStatus & TWI_APIF_bm) {   // Address/Stop Bit set
    if (wire_s->_bools._hostDataSent != 0) { // At this point, we have either a START, REPSTART or a STOP
      wire_s->_bools._hostDataSent = 0x00;
      if (wire_s->user_onReceive != NULL) {   // only if the last APIF was a Master Write,
        wire_s->user_onReceive((*head));      // we notify the sketch about new Data
      }
    }

    if (clientStatus & TWI_AP_bm) {     // Address bit set
      if ((*head) == 0) {                 // only if there was no data (START)
        pauseDeepSleep((u8)((u16)wire_s->_module));  // Only START can wake from deep sleep, change to IDLE
      }
      (*head) = 0;
      (*tail) = 0;
      (*address) = wire_s->_module->SDATA;  // read address from data register
      if (clientStatus & TWI_DIR_bm) {      // Master is reading
        if (wire_s->user_onRequest != NULL) {
          wire_s->user_onRequest();
        }
        if ((*head) == 0) {                     // If no data to transmit, send NACK
          action = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // NACK + "Wait for any Start (S/Sr) condition"
        } else {
          action = TWI_SCMD_RESPONSE_gc;        // "Execute Acknowledge Action succeeded by reception of next byte"
        }
      } else {                          // Master is writing
        wire_s->_bools._hostDataSent = 0x01;
        action = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by slave data interrupt"
      }
    } else {                          // Stop bit set
      restoreSleep((u8)((u16)wire_s->_module));
      (*head) = 0;
      (*tail) = 0;
      action = TWI_SCMD_COMPTRANS_gc;  // "Wait for any Start (S/Sr) condition"
    }
  } else if (clientStatus & TWI_DIF_bm) { // Data bit set
    if (clientStatus & TWI_DIR_bm) {        // Master is reading
      if (clientStatus & wire_s->client_irq_mask) {   // If a collision was detected, or RXACK bit is set (when it matters)
        wire_s->client_irq_mask = TWI_COLL_bm;  // stop checking for NACK
        (*head) = 0;                            // Abort further data writes
        action = TWI_SCMD_COMPTRANS_gc;         // "Wait for any Start (S/Sr) condition"
      } else {                                // RXACK bit not set, no COLL
        wire_s->_bytesTransmittedS++;           // increment bytes transmitted counter (for register model)
        wire_s->client_irq_mask = TWI_COLL_bm | TWI_RXACK_bm;  // start checking for NACK
        if ((*tail) < (*head)) {                // Data is available
          wire_s->_module->SDATA = buffer[(*tail)];  // Writing to the register to send data
          (*tail)++;                              // Increment counter for sent bytes
          action = TWI_SCMD_RESPONSE_gc;          // "Execute Acknowledge Action succeeded by reception of next byte"
        } else {                                  // No more data available
          (*head) = 0;                            // Avoid triggering REPSTART handler
          action = TWI_SCMD_COMPTRANS_gc;         // "Wait for any Start (S/Sr) condition"
        }
      }
    } else {                                  // Master is writing
      uint8_t payload = wire_s->_module->SDATA;     // reading SDATA will clear the DATA IRQ flag
      if ((*head) < TWI_BUFFER_LENGTH) {            // make sure that we don't have a buffer overflow in case Master ignores NACK
        buffer[(*head)] = payload;                  // save data
        (*head)++;                                  // Advance Head
        if ((*head) == TWI_BUFFER_LENGTH) {         // if buffer is not yet full
          action = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // "Execute ACK Action succeeded by waiting for any Start (S/Sr) condition"
        } else {                                    // else buffer would overflow with next byte
          action = TWI_SCMD_RESPONSE_gc;            // "Execute Acknowledge Action succeeded by reception of next byte"
        }
      }
    }
  }
  wire_s->_module->SCTRLB = action;  // using local variable (register) reduces the amount of loading _module
  #if defined(TWI_MANDS)
    wire_s->_bools._toggleStreamFn = 0x00;
  #endif
}

// Data handling
u8 TwoWire_write(TwoWire *self, u8 data) {
    u8 *txBuffer;
    twi_buf_index_t *txHead;

    #if defined(TWI_MANDS)  // If host and client are split
    if (self->_bools._toggleStreamFn == 0x01) {
        txHead = &(self->_bytesToReadWriteS);
        txBuffer = self->_clientBuffer;
    } else
    #endif
    {
        txHead = &(self->_bytesToReadWrite);
        txBuffer = self->_hostBuffer;
    }

    // Put byte in txBuffer
    if ((*txHead) < TWI_BUFFER_LENGTH) {    // while buffer not full, write to it
        txBuffer[(*txHead)] = data;         // Load data into the buffer
        (*txHead)++;                        // advancing the head
        return 1;
    } else {
        return 0;  // Buffer full
    }
}

// Function to write an array of bytes
u8 TwoWire_writeBytes(TwoWire *self, const u8 *data, u16 length) {
    u16 i = 0;
    for (; i < length; i++) {
        if (TwoWire_write(self, data[i]) == 0) {
            break;  // Stop if buffer is full
        }
    }
    return i;  // Number of bytes successfully written
}


int TwoWire_available(TwoWire *self) {
    return self->_bytesToReadWrite - self->_bytesReadWritten;
}

int TwoWire_read(TwoWire *self) {
    uint8_t *rxBuffer;
    twi_buf_index_t *rxHead, *rxTail;
    #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
        rxHead   = &(_bytesToReadWriteS);
        rxTail   = &(_bytesReadWrittenS);
        rxBuffer =   _clientBuffer;
    } else
    #endif
    {
        rxHead   = &(self->_bytesToReadWrite);
        rxTail   = &(self->_bytesReadWritten);
        rxBuffer =   self->_hostBuffer;
    }


    if ((*rxTail) < (*rxHead)) {   // if there are bytes to read
        uint8_t c = rxBuffer[(*rxTail)];
        (*rxTail)++;
        return c;
    } else {                      // No bytes to read. At this point, rxTail moved up to
        return -1;                  // rxHead. To reset both to 0, a MasterRead or AddrWrite has to be called
    }
}

int TwoWire_peek(TwoWire *self) {
    uint8_t *rxBuffer;
    twi_buf_index_t *rxHead, *rxTail;
    #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
        rxHead   = &(_bytesToReadWriteS);
        rxTail   = &(_bytesReadWrittenS);
        rxBuffer =   _clientBuffer;
    } else
    #endif
    {
        rxHead   = &(self->_bytesToReadWrite);
        rxTail   = &(self->_bytesReadWritten);
        rxBuffer =   self->_hostBuffer;
    }

    if ((*rxTail) < (*rxHead)) {   // if there are bytes to read
        return rxBuffer[(*rxTail)];
    } else {      // No bytes to read
        return -1;
    }
}

// Common
void TwoWire_end(TwoWire *self) {
    TwoWire_endSlave(self);
    TwoWire_endMaster(self);
}

void TwoWire_flush(TwoWire *self) {
    TWI_t* module = self->_module;
    #if defined(ERRATA_TWI_FLUSH)
    u8 temp_MCTRLA = module->MCTRLA;
    u8 temp_SCTRLA = module->SCTRLA;
    module->MCTRLA = 0x00;
    module->SCTRLA = 0x00;
    module->MCTRLA = temp_MCTRLA;
    module->MSTATUS = 0x01;
    module->SCTRLA = temp_SCTRLA;
    #else
    module->MCTRLB |= TWI_FLUSH_bm;
    #endif
}

void pauseDeepSleep(uint8_t module_lower_Addr) {
  #if defined(TWI_USING_WIRE1)
    uint8_t bit_mask = 0x10;
    if (module_lower_Addr == (uint8_t)((uint16_t)&TWI1)){
      bit_mask = 0x20;
    }
    uint8_t sleepStackLoc = sleepStack;
    if (sleepStackLoc == 0) {        // Save sleep state only if stack empty
      sleepStackLoc = SLPCTRL.CTRLA;        // save sleep settings to sleepStack
      SLPCTRL.CTRLA = sleepStackLoc & 0x01; // only leave the SEN bit, if it was set
    }
    sleepStackLoc |= bit_mask;      // Remember which module is busy
    sleepStack = sleepStackLoc;
  #else
    (void) module_lower_Addr;

    if (sleepStack == 0x00) {
      uint8_t slp = SLPCTRL.CTRLA;    // save current sleep State
      sleepStack = slp;               // using local variable for less store/load
      SLPCTRL.CTRLA = slp & 0x01;     // only leave the SEN bit, if it was set
    }
  #endif
}

void restoreSleep(uint8_t module_lower_Addr) {
  #if defined(TWI_USING_WIRE1)
    uint8_t bit_mask = ~0x10;
    if (module_lower_Addr == (uint8_t)((uint16_t)&TWI1)){
      bit_mask = ~0x20;
    }
    uint8_t sleepStackLoc = sleepStack;
    sleepStackLoc &= bit_mask;
    if (sleepStackLoc > 0) {      // only do something if sleep was enabled
      if (sleepStackLoc < 0x10) {   // if upper nibble is clear
        SLPCTRL.CTRLA = sleepStackLoc;  // restore sleep
        sleepStackLoc = 0;              // reset everything
      }
    }
    sleepStack = sleepStackLoc;
  #else
    (void) module_lower_Addr;
    SLPCTRL.CTRLA = sleepStack;
    sleepStack = 0;
  #endif
}