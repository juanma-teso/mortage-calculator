#include "mortage.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

void setCurrentDate(std::time_t* time)
{
    std::time(time);
    std::tm* result = std::localtime(time);
    result->tm_mday = 1;
    result->tm_hour = 0;
    result->tm_min = 0;
    result->tm_sec = 0;
    *time = std::mktime(result);
}
std::time_t addOneMonth(std::tm* tm)
{
    if (tm->tm_mon == 11)
    {
        tm->tm_mon = 0;
        tm->tm_year += 1;
    }
    else
    {
        tm->tm_mon += 1;
    }
    std::time_t t = std::mktime(tm);
    tm = std::localtime(&t);
    return t;
}

void showTable(AmortizationTable& at)
{
    std::stringstream ss;
    std::string nl = "\n";
    std::vector<Amortization> vec = at.GetAmortizationTable();
    std::vector<Amortization>::iterator it;

    ss << std::fixed << std::setprecision(2);
    ss << "   DATE  " << "  " << " PAYMENT" << "  " << "INTEREST" << "  " << "PRINCIPAL" << " | " << "REMAINDER" << " = " << "   DEBT   "  << " - " << "AMORTIZATION" << " " << "(PRINCIPAL" << " + " << "REDEMPTION)" << nl;
    
    for(it = vec.begin(); it != vec.end(); it++)
    {
        std::tm* date = std::localtime(&(it->date));
        ss << "  " << std::setw(2) << (date->tm_mon + 1) << "/" << (date->tm_year + 1900);
        ss << "  " << std::setw(8) << it->paymentAmount << "  " << std::setw(8) << it->interestAmount << "  " << std::setw(9) << it->principalAmount;
        ss << " | " << std::setw(9) << it->remainderAmount << " = " << std::setw(10) << it->debtAmount << " - " << std::setw(12) << (it->principalAmount + it->redemptionAmount);
        ss << " (" << std::setw(9) << it->principalAmount << " + " << std::setw(10) << it->redemptionAmount << ")" << nl;
    }
    std::cout << ss.str() << std::endl;
}

Amortization::Amortization()
{
    debtAmount = 0;
    paymentAmount = 0;
    interestAmount = 0;
    principalAmount = 0;
    redemptionAmount = 0;
    remainderAmount = 0;
    setCurrentDate(&date);
}
Amortization::Amortization(std::time_t date, double amount, double monthly, double interest, double redeem = 0)
{
    debtAmount = amount;
    paymentAmount = monthly;
    interestAmount = interest;
    principalAmount = monthly - interest;
    redemptionAmount = redeem;
    remainderAmount = debtAmount - principalAmount - redemptionAmount;
    this->date = date;
}


Redemption::Redemption(std::time_t date)
{
    redemptionAmount = 0;
    type = REDEEM_TIME;
    this->date = date;
}
Redemption::Redemption(double amount, RedemptionType redeemType, std::time_t redeemDate)
{
    redemptionAmount = amount;
    type = redeemType;
    date = redeemDate;
}
Interest::Interest(std::time_t date)
{
    anualRatePercent = 0;
    monthRatePerunit = 0;
    this->date = date;
}
Interest::Interest(std::time_t date, double anualRate)
{
    anualRatePercent = anualRate;
    monthRatePerunit = anualRate / 1200.0;
    this->date = date;
}


double AmortizationTable::calculateTaxAmount(Interest& interest)
{
    return _table.back().remainderAmount * interest.monthRatePerunit;
}
double AmortizationTable::calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments)
{
    double aux = std::pow(1 + monthlyTaxPerUnit, payments);
    return (amount * monthlyTaxPerUnit * aux) / (aux - 1);
}
Redemption AmortizationTable::getRedemption(std::time_t* date)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        if (*date == it->date) return *it;
    }
    return Redemption(*date);
}
Interest AmortizationTable::getInterest(std::time_t* date)
{
    std::vector<Interest>::iterator it;
    for (it = _interest.begin(); it != _interest.end(); it++)
    {
        if (*date == it->date) return *it;
    }
    return Interest(*date, _anualInterest);
}

AmortizationTable::AmortizationTable(double debtAmount, double anualTax, int initialPayouts)
{
    _debtAmount = debtAmount;
    _anualInterest = anualTax;
    _initialPayouts = initialPayouts;
    setCurrentDate(&_startDate);
}
std::vector<Amortization> AmortizationTable::GetAmortizationTable()
{
    double debtAmount = _debtAmount;
    double mortageAmount = calculateMortagePayment(debtAmount, _anualInterest / 1200.0, _initialPayouts);
    std::time_t t;
    std::time(&t);
    std::tm* amortizationDate = std::localtime(&t);
    Interest interest(t);
    Redemption redemption(t);

    _paidAmount = 0;
    _interestAmount = 0;
    _principalAmount = 0;
    _redeemedAmount = 0;
    _finalPayouts = 0;
    _table.clear();

    while (debtAmount > 0)
    {
        interest = getInterest(&t);
        redemption = getRedemption(&t);
        double taxAmount = calculateTaxAmount(interest);
        double redeem = calculateRedemption(redemption);

        Amortization amortization(addOneMonth(amortizationDate), debtAmount, mortageAmount, taxAmount, redeem);
        _table.push_back(amortization);

        debtAmount -= amortization.principalAmount;
        _finalPayouts ++;
        _interestAmount += amortization.interestAmount;
        _principalAmount += amortization.principalAmount;
        _redeemedAmount += amortization.redemptionAmount;
    }
    _paidAmount = _interestAmount + _principalAmount + _redeemedAmount;
    return _table;
}
void AmortizationTable::AddRedemption(Redemption redeem)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        if (redeem.date == it->date)
        {
            redeem.redemptionAmount += it->redemptionAmount;
        }
    }
    _redeems.push_back(redeem);
}
void AmortizationTable::AddRedemption(std::vector<Redemption> redeems)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        AddRedemption(*it);
    }
}
void AmortizationTable::DeleteRedemption(Redemption redeem)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        if (*it == redeem)
        {
            _redeems.erase(it);
        }
    }
}
void AmortizationTable::DeleteRedemption(std::time_t date)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        if (it->date == date)
        {
            _redeems.erase(it);
        }
    }
}
void AmortizationTable::DeleteRedemptions()
{
    _redeems.clear();
}
std::vector<Redemption> AmortizationTable::GetRedemptions()
{
    return _redeems;
}



int main (void)
{
    double amount = 68000.0;
    double anualTaxPercent = 3;
    int years = 25;

    AmortizationTable table(amount, anualTaxPercent, years*12);

    std::cout << "\n" << "AMORTIZATION TABLE (AMOUNT: " << table.GetDebtAmount() << " INTEREST: " << table.GetAnualInterest() << " PAYOUTS: " << table.GetPayoutsNum() << ")\n\n";
    showTable(table);
    std::cout << "\n" << table << std::endl;
    return 0;
}


