#include <mortage.hpp>
#include <cmath>

std::tm* getCurrentDate()
{
    std::tm* result = std::localtime(nullptr);
    result->tm_mday = 1;
    result->tm_hour = 0;
    result->tm_min = 0;
    result->tm_sec = 0;
    std::mktime(result);
    return result;
}
std::tm* addOneMonth(std::tm& tm)
{
    tm.tm_mon = tm.tm_mon == 11 ? 0 : tm.tm_mon + 1;
    return &tm;
}

Amortization::Amortization()
{
    debtAmount = 0;
    paymentAmount = 0;
    interestAmount = 0;
    amortizationAmount = 0;
    redemptionAmount = 0;
    remainderAmount = 0;
    date = getCurrentDate();
}
Amortization::Amortization(double amount, double monthly, double interest, double redeem = 0)
{
    debtAmount = amount;
    paymentAmount = monthly;
    interestAmount = interest;
    amortizationAmount = monthly - interest;
    redemptionAmount = redeem;
    remainderAmount = debtAmount - amortizationAmount - redemptionAmount;
    date = getCurrentDate();
}

Redemption::Redemption()
{
    redemptionAmount = 0;
    type = AMOUNT;
    date = getCurrentDate();
}
Redemption::Redemption(double amount, RedemptionType redeemType, std::tm& redeemDate)
{
    redemptionAmount = amount;
    type = redeemType;
    date = &redeemDate;
}

double AmortizationTable::calculateTaxAmount(double amount, double monthlyTaxPerUnit)
{
    return amount * monthlyTaxPerUnit;
}
double AmortizationTable::calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments)
{
    double aux = std::pow(1 + monthlyTaxPerUnit, payments);
    return (amount * monthlyTaxPerUnit * aux) / (aux - 1);
}

AmortizationTable::AmortizationTable(double debtAmount, double anualTax, int initialPayouts)
{
    _debtAmount = debtAmount;
    _anualTax = anualTax;
    _initialPayouts = initialPayouts;
}

std::vector<Amortization> AmortizationTable::GetAmortizationTable()
{
    double debtAmount = _debtAmount;
    double mortageAmount = calculateMortagePayment(debtAmount, _anualTax / 1200.0, _initialPayouts);

    while (debtAmount > 0)
    {
        double taxAmount = calculateTaxAmount(debtAmount, _anualTax / 1200.0);
        Amortization amortization(debtAmount, mortageAmount, taxAmount);
    }
}


int main (void)
{
    double amount = 68000.0;
    double anualTaxPercent = 3;
    int years = 25;

    AmortizationTable table(amount, anualTaxPercent, years*12);
    table.GetAmortizationTable();
}


