#include "mortage.hpp"
#include <cmath>
#include <iostream>
#include <sstream>

Date getCurrentDate()
{
    std::time_t time;
    std::time(&time);
    std::tm* result = std::localtime(&time);
    result->tm_mday = 1;
    result->tm_hour = 0;
    result->tm_min = 0;
    result->tm_sec = 0;
    return Date(result->tm_year+1900, result->tm_mon+1);
}

Date::Date()
{
    Date d = getCurrentDate();
    year = d.year;
    month = d.month;
}
Date::Date(int year, int month)
{
    this->year = year;
    this->month = month;
}

void showTable(AmortizationTable& at)
{
    std::stringstream ss;
    std::string nl = "\n";
    std::vector<Amortization> vec = at.GetAmortizationTable();
    std::vector<Amortization>::iterator it;

    ss << std::fixed << std::setprecision(2);
    ss << "   DATE  " << "  " << " PAYMENT" << "  " << "INTEREST" << "  " << "PRINCIPAL" << " | " << "   DEBT   "  << " - " << "AMORTIZATION" << " " << "(PRINCIPAL" << " + " << "REDEMPTION)" << " = " << "REMAINDER" << nl;
    
    for(it = vec.begin(); it != vec.end(); it++)
    {
        ss << "  " << std::setw(8) << it->date;
        ss << "  " << std::setw(8) << it->paymentAmount << "  " << std::setw(8) << it->interestAmount << "  " << std::setw(9) << it->principalAmount;
        ss << " | " << std::setw(10) << it->debtAmount << " - " << std::setw(12) << (it->principalAmount + it->redemptionAmount);
        ss << " (" << std::setw(9) << it->principalAmount << " + " << std::setw(10) << it->redemptionAmount << ")" << " = "  << std::setw(9) << it->remainderAmount << nl;
    }
    std::cout << ss.str() << std::endl;
}

Redemption::Redemption(Date date)
{
    redemptionAmount = 0;
    type = REDEEM_TIME;
    this->date = date;
}
Redemption::Redemption(Date redeemDate, double amount, RedemptionType redeemType)
{
    redemptionAmount = amount;
    type = redeemType;
    date = redeemDate;
}
Interest::Interest(Date date)
{
    anualRatePercent = 0;
    monthRatePerunit = 0;
    this->date = date;
}
Interest::Interest(Date date, double anualRate)
{
    anualRatePercent = anualRate;
    monthRatePerunit = anualRate / 1200.0;
    this->date = date;
}

Amortization::Amortization()
{
    debtAmount = 0;
    paymentAmount = 0;
    interestAmount = 0;
    principalAmount = 0;
    redemptionAmount = 0;
    remainderAmount = 0;
    date = getCurrentDate();
}
Amortization::Amortization(Date date, double amount, double monthly, double interest, double redeem = 0)
{
    this->date = date;
    debtAmount = amount;
    paymentAmount = monthly;
    interestAmount = interest;
    principalAmount = monthly - interest;
    redemptionAmount = redeem;
    remainderAmount = debtAmount - principalAmount - redemptionAmount;
    if (remainderAmount < 0.0)
    {
        remainderAmount = 0;
        principalAmount = principalAmount > debtAmount ? debtAmount : principalAmount;
        redemptionAmount = principalAmount > debtAmount ? 0 : debtAmount - principalAmount;
    }
}
double AmortizationTable::calculateRedemption(Redemption& redemption)
{
    return redemption.redemptionAmount;
}
double AmortizationTable::calculateTaxAmount(Interest& interest)
{
    double remainder = _table.size() > 0 ? _table.back().remainderAmount : _debtAmount;
    return remainder * interest.monthRatePerunit;
}
double AmortizationTable::calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments)
{
    double aux = std::pow(1 + monthlyTaxPerUnit, payments);
    return (amount * monthlyTaxPerUnit * aux) / (aux - 1);
}
Redemption AmortizationTable::getRedemption(Date* date)
{
    std::vector<Redemption>::iterator it;
    for (it = _redeems.begin(); it != _redeems.end(); it++)
    {
        if (*date == it->date) return *it;
    }
    return Redemption(*date);
}
Interest AmortizationTable::getInterest(Date* date)
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
}
std::vector<Amortization> AmortizationTable::GetAmortizationTable()
{
    return _table;
}
void AmortizationTable::CalculateAmortization()
{
    double debtAmount = _debtAmount;
    double mortageAmount = calculateMortagePayment(debtAmount, _anualInterest / 1200.0, _initialPayouts);
    Date amortizationDate = _startDate;
    Interest interest(amortizationDate);
    Redemption redemption(amortizationDate);

    _paidAmount = 0;
    _interestAmount = 0;
    _principalAmount = 0;
    _redeemedAmount = 0;
    _finalPayouts = 0;
    _table.clear();

    while (debtAmount > 0)
    {
        interest = getInterest(&amortizationDate);
        redemption = getRedemption(&amortizationDate);
        double taxAmount = calculateTaxAmount(interest);
        double redeem = calculateRedemption(redemption);

        Amortization amortization(amortizationDate, debtAmount, mortageAmount, taxAmount, redeem);
        _table.push_back(amortization);

        debtAmount -= amortization.principalAmount + amortization.redemptionAmount;
        amortizationDate += 1;
        _finalPayouts ++;
        _interestAmount += amortization.interestAmount;
        _principalAmount += amortization.principalAmount;
        _redeemedAmount += amortization.redemptionAmount;
        if (redemption.type == REDEEM_AMOUNT)
        {
            mortageAmount = calculateMortagePayment(debtAmount, interest.monthRatePerunit, _initialPayouts - _finalPayouts);
        }
    }
    _paidAmount = _interestAmount + _principalAmount + _redeemedAmount;
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
void AmortizationTable::DeleteRedemption(Date date)
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
    Date redeemT;
    AmortizationTable table(amount, anualTaxPercent, years*12);

    table.DeleteRedemptions();
    table.CalculateAmortization();
    std::cout << "NO AMORTIZATION:\n" << table << std::endl;
    showTable(table);
    
    redeemT = table.GetStartDate();
    for (int index = 0; index < 24; index++)
    {
        table.AddRedemption(Redemption(redeemT, 400, REDEEM_AMOUNT));
        redeemT += 1;
    }
    table.CalculateAmortization();
    std::cout << "AMORTIZATION: 24 * 400 | AMOUNT:\n" << table << std::endl;
    showTable(table);

    table.DeleteRedemptions();
    redeemT = table.GetStartDate();
    for (int index = 0; index < 24; index++)
    {
        table.AddRedemption(Redemption(redeemT, 400, REDEEM_TIME));
        redeemT += 1;
    }
    table.CalculateAmortization();
    std::cout << "AMORTIZATION: 24 * 400 | TIME:\n" << table << std::endl;
    showTable(table);

    table.DeleteRedemptions();
    redeemT = table.GetStartDate();
    for (int index = 0; index < table.GetPayoutsNum(); index++)
    {
        table.CalculateAmortization();
        std::vector<Amortization> am = table.GetAmortizationTable();
        if (index < 24)
        {
            table.AddRedemption(Redemption(redeemT, 500, REDEEM_TIME));
        }
        else
        {
            table.AddRedemption(Redemption(redeemT, 180, REDEEM_TIME));
        }
        redeemT += 1;
    }
    table.CalculateAmortization();
    std::cout << "AMORTIZATION: 24 * 500 -> 500 - payment | TIME:\n" << table << std::endl;
    showTable(table);

    table.DeleteRedemptions();
    redeemT = table.GetStartDate();
    for (int index = 0; index < table.GetPayoutsNum(); index++)
    {
        table.CalculateAmortization();
        std::vector<Amortization> am = table.GetAmortizationTable();
        if (index < 12)
        {
            table.AddRedemption(Redemption(redeemT, 500, REDEEM_TIME));
        }
        else
        {
            table.AddRedemption(Redemption(redeemT, 700 - am[index].paymentAmount, REDEEM_TIME));
        }
        redeemT += 1;
    }
    table.CalculateAmortization();
    std::cout << "AMORTIZATION: 12 * 500 -> 60 * 700 - payment | TIME:\n" << table << std::endl;
    showTable(table);


    
    return 0;
}


