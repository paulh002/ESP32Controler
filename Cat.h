#pragma once
#include <string>
#include "FT891_CAT.h"
#include "Gui_band.h"


class Comm : public Cat_communicator
{
public:
	void	Send(std::string s) override;
	void	Read(char c, std::string& s) override;
	bool	available() override;
	void	SendInformation(int info) override;
	void	Processinformation(std::string s) override;
	void	setband(int band) override;
};

class Cat
{
public:
	bool	checkCAT();
	void	begin();
	void	Setft(int ft);
	void	Setag(int ag);
	void	SetIf(int if_filter);
	int32_t GetIf();
	uint8_t	Getag();
	long long	Getfa();
	void	Setrg(int rg);
	uint8_t	Getrg();
	void	Settx(int tx);
	void	SetBnd(int bnd);
	int		GetSM();
	void	Requestinformation(int info);
	uint8_t Getig();
	void	Setig(int ig);
	int		GetTX();

private:
	FT891_CAT	cat_message;
	bool		f_rxtx;
	Comm		cat_comm;
	long long	frequency_a;
	int			lastCat;

	void PollSM();
};

extern Cat CatInterface;