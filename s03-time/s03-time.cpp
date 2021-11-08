/*
	Zrobiłem z obsługą znaków UTF-8
*/

#include <iostream>
#include <sstream>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <iomanip>

using std::wcout;
using std::wcerr;
using std::endl;
using std::wstring;
using std::wstringstream;
using std::exception;
using std::setw;
using std::setfill;

enum class Time_of_day
{
	Rano,
	Dzien,
	Wieczor,
	Noc
};

struct Time
{
	int64_t total;

	unsigned char hour;
	unsigned char minute;
	unsigned char second;

	Time(unsigned char hour, unsigned char minute, unsigned char second) : total(0)
	{
		if (hour > 23) { throw exception{ "Niepoprawna ilość godzin" }; }
		if (minute > 59) { throw exception{ "Niepoprawna ilość minut" }; }
		if (second > 59) { throw exception{ "Niepoprawna ilość sekund" }; }

		this->hour = hour;
		this->minute = minute;
		this->second = second;

		hms2total();
	}

	/* Konstruktor pomocniczy, ułatwi mi operacje na operatorach */
	Time(int64_t total)
	{
		/* 
			Jeżeli dostaniemy ujemny Time, zwiększamy go o liczbe sekund jaka jest w 1 dniu, aż do momentu uzyskania liczby dodatniej
			Jest to potrzebne dla operatora odejmowania..
		*/
		while (total < 0)
		{
			total += 60 * 60 * 24;
		}

		this->total = total;
		total2hms();
	}

	auto time_of_day() const -> Time_of_day
	{
		if (hour >= 0 && hour < 6) 
		{
			return Time_of_day::Noc;
		}

		if (hour >= 6 && hour < 12)
		{
			return Time_of_day::Rano;
		}

		if (hour >= 12 && hour < 18)
		{
			return Time_of_day::Dzien;
		}

		if (hour >= 18 && hour < 24)
		{
			return Time_of_day::Wieczor;
		}
	}

	auto count_seconds() const -> uint64_t
	{
		return (uint64_t)total;
	}

	auto count_minutes() const -> uint64_t
	{
		return (uint64_t)(total / 60);
	}

	auto time_to_midnight() const -> Time
	{
		return Time{ 60 * 60 * 24 - total };
	}

	auto next_hour() -> void
	{
		if (hour + 1 == 24)
		{
			hour = 0;
		}
		else
		{
			hour++;
		}
		hms2total();
	}

	auto next_minute() -> void
	{
		if (minute + 1 == 60)
		{
			minute = 0;
			next_hour();
		}
		else
		{
			minute++;
			hms2total();
		}
	}

	auto next_second() -> void
	{
		if (second + 1 == 60)
		{
			second = 0;
			next_minute();
		}
		else
		{
			second++;
			hms2total();
		}
	}

#pragma region Funkcje pomocnicze
	/* Zamiana godzin, minut i sekund na same sekundy */
	auto hms2total() -> void
	{
		total = (uint64_t)(hour * 3600 + minute * 60 + second);
		total %= 60 * 60 * 24;
	}

	/* Zamiana samych sekund na godzine, minuty i sekundy */
	auto total2hms() -> void
	{
		second = total % 60;
		minute = (int)(total / 60) % 60;
		hour = (int)(total / 3600) % 24;
	}
#pragma endregion

#pragma region Opeartory
	auto operator+ (Time const& obj) const -> Time
	{
		return Time{ this->total + obj.total };
	}

	auto operator- (Time const& obj) const -> Time
	{
		return Time{ this->total - obj.total };
	}

	auto operator< (Time const& obj) const -> bool
	{
		return this->total < obj.total;
	}

	auto operator> (Time const& obj) const -> bool
	{
		return this->total > obj.total;
	}

	auto operator== (Time const& obj) const -> bool
	{
		return this->total == obj.total;
	}

	auto operator!= (Time const& obj) const -> bool
	{
		return this->total != obj.total;
	}
#pragma endregion

	auto to_string() const -> wstring
	{
		wstringstream stream;
		stream << setfill(L'0') << setw(2) << hour << ":"
			<< setfill(L'0') << setw(2) << minute << ":"
			<< setfill(L'0') << setw(2) << second;

		return stream.str();
	}

	auto to_string(Time_of_day time_of_day) const -> wstring
	{
		switch (time_of_day)
		{
		case Time_of_day::Rano:
			return L"Rano";

		case Time_of_day::Dzien:
			return L"Dzień";

		case Time_of_day::Wieczor:
			return L"Wieczór";

		case Time_of_day::Noc:
			return L"Noc";

		default:
			return L"Nieznany time of day";
		}
	}
};

auto main() -> int
{
	try
	{
		if (_setmode(_fileno(stdout), _O_U8TEXT) == -1)
		{
			throw exception{ "Nie można ustawić kodowania UTF-8" };
		}

#pragma region Zadanie: Czas
		Time Czas{ 23,59,59 };

		wcout << L"[Zadanie: Czas]" << endl;
		wcout << L"Czas: " << Czas.to_string() << endl;
		Czas.next_minute();
		wcout << L"Zwiększono czas o minutę!" << endl;
		wcout << L"Czas: " << Czas.to_string() << endl << endl;
#pragma endregion

#pragma region Zadanie: Pora dnia
		wcout << L"[Zadanie: Pora dnia]" << endl;
		wcout << L"Czas: " << Czas.to_string() << endl;
		wcout << L"Pora dnia to " << Czas.to_string(Czas.time_of_day()) << endl << endl;
#pragma endregion

#pragma region Zadanie: Arytmetyka
		Time Czas1{ 1,15,1 };
		Time Czas2{ 8,30,25 };

		wcout << L"[Zadanie: Arytmetyka]" << endl;

		wcout << "Czas1: " << Czas1.to_string() << endl;
		wcout << "Czas2: " << Czas2.to_string() << endl;

		wcout << L"Czas1 + Czas2 = " << (Czas1 + Czas2).to_string() << endl;
		wcout << L"Czas1 - Czas2 = " << (Czas1 - Czas2).to_string() << endl;
		wcout << L"Czas1 jest mniejszy od Czas2: " << ((Czas1 < Czas2) ? L"Prawda!" : L"Fałsz!") << endl;
		wcout << L"Czas1 jest większy od Czas2: " << ((Czas1 > Czas2) ? L"Prawda!" : L"Fałsz!") << endl;
		wcout << L"Czas1 jest równy od Czas2: " << ((Czas1 == Czas2) ? L"Prawda!" : L"Fałsz!") << endl;
		wcout << L"Czas1 jest nierówny od Czas2: " << ((Czas1 != Czas2) ? L"Prawda!" : L"Fałsz!") << endl << endl;
#pragma endregion

#pragma region Zadanie: Sekundy do północy
		wcout << L"[Zadanie: Sekundy do północy]" << endl;
		wcout << L"Czas: " << Czas1.to_string() << endl;
		wcout << L"Sekundy: " << Czas1.count_seconds() << endl;
		wcout << L"Minuty: " << Czas1.count_minutes() << endl;
		wcout << L"Czas do północy: " << Czas1.time_to_midnight().to_string() << endl << endl;
#pragma endregion

	}
	catch (exception ex)
	{
		wcerr << "Wyjątek: " << ex.what() << endl;
		return 1;
	}

	return 0;
}