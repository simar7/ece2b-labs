-- ECE 324 - Lab 0
-- VHDL Design of a Full Adder
--
-- William D. Bishop
-- Department of Electrical and Computer Engineering
-- University of Waterloo, Waterloo, Ontario, CANADA
-- N2L 3G1

-- The following lines include the definitions necessary to use
-- the STD_LOGIC and STD_LOGIC_VECTOR types that are commonly used
-- in VHDL.  The IEEE library also defines the operation of
-- the XOR, OR, and AND functions used in this design.
LIBRARY ieee;
USE ieee.std_logic_1164.all;

-- The entity declaration defines the INTERFACE to the design.  It 
-- specifies the PORTS (pins) and the GENERICS (parameters) to 
-- the design.  A full adder design has five pins as shown below.
-- This design does not require any GENERICS.
ENTITY full_adder IS

	PORT
	(
		a		: IN	STD_LOGIC;
		b		: IN	STD_LOGIC;
		cin		: IN	STD_LOGIC;
		sum		: OUT	STD_LOGIC;
		cout	: OUT	STD_LOGIC
	);
	
END full_adder;

-- The architecture declaration defines the IMPLEMENTATION of the
-- design.  You can have multiple architecture declarations for
-- complex VHDL designs.  For example, you might have an 
-- architecture optimized for performance and another optimized
-- for efficient device utilization.
ARCHITECTURE REV001 OF full_adder IS

BEGIN

	-- These two equations specify the functionality of a full adder
	-- using combinational logic.  These statements are concurrent
	-- so the order in which they appear is irrelevant.
	sum <= a XOR b XOR cin;
	cout <= (a AND b) OR (a AND cin) OR (b AND cin);
	
END REV001;