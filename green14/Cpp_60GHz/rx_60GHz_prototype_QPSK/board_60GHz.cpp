
//
// Copyright Per Zetterberg, KTH Royal Institute of Technology
//
//

#include "board_60GHz.hpp"
#include <iostream>



board_60GHz_base::board_60GHz_base(uhd::usrp::dboard_iface::sptr db_iface,
		      uhd::usrp::dboard_iface::unit_t unit,
		 uint16_t enable_hmc, uint16_t data_in_hmc,
				   uint16_t clk_hmc, uint16_t data_out_hmc, uint16_t reset_hmc, uint16_t chip_addr )
{


  std::vector<uint16_t> gpio_lines;

  m_dummy=15; // We do dummy reads from this line.

  gpio_lines.push_back(enable_hmc);
  gpio_lines.push_back(data_in_hmc);
  gpio_lines.push_back(clk_hmc);
  gpio_lines.push_back(data_out_hmc);
  gpio_lines.push_back(reset_hmc);

  for (uint32_t i1=0;i1<gpio_lines.size();i1++) {
    if (gpio_lines[i1]==m_dummy) {
      std::cerr << "Use of GPIO line " << m_dummy << "is not allowed \n";
      exit(2);
    };
  };  
  sort(gpio_lines.begin(),gpio_lines.end());
  for (uint32_t i1=0;i1<gpio_lines.size()-1;i1++) {
    if (gpio_lines[i1]==gpio_lines[i1+1]) {
      std::cerr << "GPIO lines must be distinct \n";
      exit(1);
    };
  };


  m_db_iface=db_iface;
  m_unit=unit;
  m_enable_hmc=enable_hmc;
  m_data_in_hmc=data_in_hmc;
  m_clk_hmc=clk_hmc;
  m_data_out_hmc=data_out_hmc;
  m_reset_hmc=reset_hmc;
  m_chip_addr=chip_addr;

  mask_write=(1<<m_enable_hmc) | (1<<m_data_out_hmc) | (1<<m_clk_hmc) | 
    (m_reset_hmc);
  mask_read=(1<<m_data_in_hmc) | (1<<m_dummy);
  mask_all=mask_write | mask_read;


  m_db_iface->set_pin_ctrl(m_unit,0,mask_all);
  m_db_iface->set_gpio_ddr(m_unit,0,mask_read);
  m_db_iface->set_gpio_ddr(m_unit,mask_write,mask_write);
  
  // Set enable high and CLK low
  m_db_iface->set_gpio_out(m_unit,1 << m_enable_hmc,1 << m_enable_hmc ); 
  m_db_iface->set_gpio_out(m_unit,0,1 << m_clk_hmc); 


  /*  
  while (1) {

  m_db_iface->set_gpio_out(m_unit,1 << m_enable_hmc,1 << m_enable_hmc ); 
  usleep(1e3);
  m_db_iface->set_gpio_out(m_unit,0,1 << m_enable_hmc ); 
  usleep(1e3);
  };
  */
  



  // Let reset go high
  m_db_iface->set_gpio_out(m_unit,1<< m_reset_hmc,1 << m_reset_hmc); 
  usleep(2e5);
  m_db_iface->set_gpio_out(m_unit,0,1 << m_reset_hmc); 

};
  
void board_60GHz_base::write_row(uint16_t row_num, uint32_t value) {

  uint32_t total_write;
  int16_t data;
  double T=1000; // Use to increase clock period
  
  //uint32_t value_brs; // Bit reversed value
  uint32_t row_num_brs; // Bit reversed row number

  row_num=row_num & 0xF;
  value=value & 0xFF;

  row_num_brs=0;
  for (int i1=0;i1<6;i1++)
    row_num_brs=row_num_brs | (((row_num>>i1) & 1) << (5-i1));

  //value_brs=0;
  //for (int i1=0;i1<8;i1++)
  //  value_brs=value_brs | (((value >>i1) & 1) << (7-i1));

  //total_write=(1<<14) | (0<<15) | (1<<16) | (1<<17); // R/W +chip address
  //total_write=(1<<14) | (1<<15) | (1<<16) | (1<<17); // R/W +chip address
  total_write=(1<<14) | (m_chip_addr<<15);

  total_write=total_write | value;
  total_write=total_write | (row_num << 8);


  // Set enable low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_enable_hmc);
  usleep(T/2);

  for (int i1=0;i1<=17;i1++) {
      //std::cout << "i1=" << i1 << std::endl;
      // Clock low
      m_db_iface->set_gpio_out(m_unit,0,1 << m_clk_hmc);
      // Write data
      data=(total_write >> i1) & 1;
      //std::cout << "data=" << (data << m_data_sky) << std::endl;
      m_db_iface->set_gpio_out(m_unit,data << m_data_out_hmc,
			       1 << m_data_out_hmc);
      // Dummy read
      m_db_iface->read_gpio(m_unit);
      usleep(T/2);
      // Clock high
      m_db_iface->set_gpio_out(m_unit,1 << m_clk_hmc, 1 << m_clk_hmc);
      // Dummy read
      m_db_iface->read_gpio(m_unit);
      usleep(T/2);

  };
  // Clock low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_clk_hmc);
  // Data  low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_data_out_hmc);
      

  // LE high
  usleep(T/2);
  m_db_iface->set_gpio_out(m_unit,1 << m_enable_hmc,1 << m_enable_hmc);
  usleep(T*100);

};


uint16_t board_60GHz_base::read_row(uint16_t row_num) {
 
   uint16_t value=0;

  uint32_t total_write;
  int16_t data;
  double T=1000; // Use to increase clock period
  
  //uint32_t value_brs; // Bit reversed value
  uint32_t row_num_brs; // Bit reversed row number


  row_num=row_num & 0xF;
  value=value & 0xFF;

  row_num_brs=0;

  for (int i1=0;i1<6;i1++) {
    row_num_brs=row_num_brs | (((row_num>>i1) & 1) << (5-i1));
  };

  
  //value_brs=0;
  //for (int i1=0;i1<8;i1++)
  //  value_brs=value_brs | (((value >>i1) & 1) << (7-i1));


  //total_write=(0<<14) | (0<<15) | (1<<16) | (1<<17); // R/W +chip address
  //total_write=(0<<14) | (1<<15) | (1<<16) | (1<<17); // R/W +chip address
  total_write=(0<<14) | (m_chip_addr<<15);
  total_write=total_write | value;
  total_write=total_write | (row_num << 8);


 
  // Set enable low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_enable_hmc);
  usleep(T/2);

  for (int i1=0;i1<=17;i1++) {
      //std::cout << "i1=" << i1 << std::endl;
      // Clock low
      m_db_iface->set_gpio_out(m_unit,0,1 << m_clk_hmc);
      // Write data
      data=(total_write >> i1) & 1;
      //std::cout << "data=" << (data << m_data_sky) << std::endl;
      m_db_iface->set_gpio_out(m_unit,data << m_data_out_hmc,
			       1 << m_data_out_hmc);
      // Dummy read
      m_db_iface->read_gpio(m_unit);
      usleep(T/2);
      // Clock high
      m_db_iface->set_gpio_out(m_unit,1 << m_clk_hmc, 1 << m_clk_hmc);
      // Dummy read
      m_db_iface->read_gpio(m_unit);
      usleep(T/2);

  };

  // Clock low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_clk_hmc);
  // Data low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_data_out_hmc);
  usleep(T/2);
  // Set enable high
  m_db_iface->set_gpio_out(m_unit,1 << m_enable_hmc,1 << m_enable_hmc);
  usleep(T/2);
  // Clock high
  m_db_iface->set_gpio_out(m_unit,1 << m_clk_hmc, 1 << m_clk_hmc);
  usleep(T/2);
  // Clock low
  m_db_iface->set_gpio_out(m_unit,0, 1 << m_clk_hmc);
  usleep(T/2*3);
  // Enable low
  m_db_iface->set_gpio_out(m_unit,0,1 << m_enable_hmc);
  usleep(T/2);     

  uint32_t temp;
  for (int i1=0;i1<8;i1++) {
    // Clock high
    m_db_iface->set_gpio_out(m_unit,1 << m_clk_hmc, 1 << m_clk_hmc);
    usleep(T/2);
    temp=m_db_iface->read_gpio(m_unit); 
    temp=(temp >> m_data_in_hmc) & 1;
    if (temp==1)
      temp=0;
    else
      temp=1;
    value=value | (temp << i1);
    // Clock low
    m_db_iface->set_gpio_out(m_unit,0, 1 << m_clk_hmc);
    usleep(T/2);
  };
  

  // LE high
  usleep(T);
  m_db_iface->set_gpio_out(m_unit,1 << m_enable_hmc,1 << m_enable_hmc);

  return value;

};


#define DATA_OUT_HMC 1
#define DATA_IN_HMC 0
#define CLK_HMC 2
#define ENABLE_HMC 3
#define RESET_HMC 4


board_60GHz_TX::board_60GHz_TX(uhd::usrp::dboard_iface::sptr db_iface) :
board_60GHz_base(db_iface,uhd::usrp::dboard_iface::UNIT_TX,
				   ENABLE_HMC, DATA_IN_HMC, CLK_HMC,
		 DATA_OUT_HMC, RESET_HMC,2+4) {


    write_row(0,0); // Power on everything
    write_row(1,0); // Power on and highest Q of filter.
    write_row(2,240); // Taken from PC app.
    write_row(3,31); // Taken from PC app.
    write_row(4,63); // Normal operation
    write_row(5,244); // Normal operation
    write_row(6,143); // 

    int l_tx_gain=13; // 0:13. Increasing gain.
    write_row(7,15+16*(13-l_tx_gain)); 
                                   // Highest gain + normal operation
    
    write_row(8,191); // normal operation
    write_row(9,111); // normal operation


    // Table 10. 285.7143MHz Reference
    // Frequency(GHz)    DIVRATIO            BAND

    /*      57             00001             000
            57.5           00010             000
            58             00011             001
            58.5           00100             001
            59             00101             010
            59.5           00110             010
            60             00111             011
            60.5           01000             011
            61             01001             100
            61.5           01010             100
            62             01011             101
            62.5           01100             101
            63             01101             110
            63.5           01110             110
            64             01111             111  */


    write_row(10,240); // 240+DIVRATIO<4>
    write_row(11,16*(1+2+4)+2*3+1); // 16*DIVRATIO<3:0>+2*BAND+1


    write_row(12,95); // Syntesizer parameters (lock window)
    write_row(13,128); // normal operation (synths on)
    write_row(14,118); // normal operation

    #if 0
    for (int i1=0;i1<15;i1++) {
      std::cout << "reg=" << i1 << " value=" << read_row(i1) 
          << "\n";
    };
    #endif

    std::cout << "Waiting for PLL lock \n";
    int lock=read_row(15)>> 6;
    while (lock!=1) {
      std::cout << ".";
      usleep(1e6);
      lock=read_row(15)>> 6;
    };
    std::cout << "PLL has locked! \n";
   
}

void board_60GHz_TX::set_gain(uint16_t tx_gain) {
  if (tx_gain>13)
    tx_gain=13;
 
  write_row(7,15+16*(13-tx_gain)); 

};


board_60GHz_RX::board_60GHz_RX(uhd::usrp::dboard_iface::sptr db_iface) :
board_60GHz_base(db_iface,uhd::usrp::dboard_iface::UNIT_TX,
				   ENABLE_HMC, DATA_IN_HMC, CLK_HMC,
		 DATA_OUT_HMC, RESET_HMC,1+2+4) {


    write_row(0,128); // Everthing on except ASK mod.
    int bb_gain1=0; // 0-3
    int bb_gain2=0; // 0-3
    int bb_att1=3-bb_gain1;
    int bb_att2=3-bb_gain2;
    write_row(1,bb_att2+4*bb_att1); // Power on + gain control

    int bb_gain_fineI=0; // 0-5
    int bb_gain_fineQ=0; // 0-5

    int bb_att_fineI=5-bb_gain_fineI;
    int bb_att_fineQ=5-bb_gain_fineQ;


    write_row(2,4*bb_att_fineQ+32*bb_att_fineI); 
                                               // Normal operation

    int bb_low_pass_corner=3; // 0=>1.4GHz, 1=>500MHz, 2=> 300MHz, 3=>200MHz.
    int bb_high_pass_corner=2; // 0=>30kHz, 1=>300kHz, 2=>1.5MHz.

    write_row(3,3+16*bb_high_pass_corner+64*bb_low_pass_corner);
                                            // Normal operation
    
    write_row(4,158); // Normal operation

    int if_gain=15; // 0-15
    int if_att=15-if_gain;

    write_row(5,15+16*if_att); // Normal operation
    write_row(6,191); // Normal operation
    write_row(7,109); // Normal operation
    write_row(8,128); // Normal operation
    write_row(9,0); // Normal operation
    write_row(10,240); // 240+DIVRATIO<4>
    write_row(11,16*(1+2+4)+2*3+1); // 16*DIVRATIO<3:0>+2*BAND+1
    write_row(12,95); // Normal operation
    write_row(13,128); // Normal operation
    write_row(14,118); // Normal operation

    
    #if 0
    for (int i1=0;i1<15;i1++) {
      std::cout << "reg=" << i1 << " value=" << read_row(i1) 
          << "\n";
    };
    #endif

    std::cout << "Waiting for PLL lock \n";
    int lock=read_row(15)>> 6;
    while (lock!=1) {
      std::cout << ".";
      usleep(1e6);
      lock=read_row(15)>> 6;
    };
    std::cout << "PLL has locked! \n";


  


}

void board_60GHz_RX::set_gain(uint16_t rx_gain) {
  if (rx_gain>15)
    rx_gain=15;

   int if_att=15-rx_gain;
   write_row(5,15+16*if_att); // Normal operation

};

