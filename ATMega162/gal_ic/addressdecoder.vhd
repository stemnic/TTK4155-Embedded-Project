library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity address_decoder is
    Port (
        a11, a10, a9, a8 : in std_logic;
        ram_cs, adc_cs, oled_cs, oled_dc: out std_logic
    );
    attribute LOC : string;
    attribute LOC of ram_cs  : signal is "P19";
    attribute LOC of adc_cs  : signal is "P18";
    attribute LOC of oled_cs : signal is "P17";
    attribute LOC of oled_dc : signal is "P16";
    
    attribute LOC of a11     : signal is "P1";
    attribute LOC of a10     : signal is "P2";
    attribute LOC of a9      : signal is "P3";
    attribute LOC of a8      : signal is "P4";
end address_decoder;

architecture behavioral of address_decoder is
begin
    ram_cs <= a11;
    adc_cs <= (not a11) and a10;
    oled_cs <= (not a11) and (not a10) and a9;
    oled_dc <= (not a11) and (not a10) and (not a9);
end behavioral;

