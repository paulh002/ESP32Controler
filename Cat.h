#pragma once
#include <string>
#include "FT891_CAT.h"

class Comm : public Cat_communicator
{
public:
	void	Send(std::string s, bool addNewline) override;
	void	Read(char c, std::string& s) override;
	bool	available() override;
};

class Cat
{
public:
	void	checkCAT();
	void	begin();
	void	Setft(int ft);
	void	Setag(int ag);
	uint8_t	Getag();
	long long	Getfa();
	void	Setrg(int rg);
	uint8_t	Getrg();
	void	Settx(int tx);


private:
	FT891_CAT	cat_message;
	bool		f_rxtx;
	Comm		cat_comm;

	int			afgain;
	int			rfgain;
	long long	frequency_a;
};

