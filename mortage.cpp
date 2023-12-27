#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

struct Amortization
{
    double paymentAmount;       //Cuota
    double interestAmount;      //Interés
    double amortizationAmount;  //Principal
    double redemptionAmount;    //Amortización anticipada

    double debtAmount;          //Deuda base
    double remainderAmount;     //Deuda pendiente

    Amortization()
    {
        debtAmount = 0;
        paymentAmount = 0;
        interestAmount = 0;
        amortizationAmount = 0;
        redemptionAmount = 0;
        remainderAmount = 0;
    }
    Amortization(double amount, double monthly, double interest, double redeem = 0)
    {
        debtAmount = amount;
        paymentAmount = monthly;
        interestAmount = interest;
        amortizationAmount = monthly - interest;
        redemptionAmount = redeem;
        remainderAmount = debtAmount - amortizationAmount - redemptionAmount;
    }
};

class AmortizationTable
{
    private:
        std::vector<Amortization> table;
        double _debtAmount;
        double _anualTax;
        int _initialPayouts;

        double _paidAmount;
        double _interestAmount;
        double _capitalAmount;
        double _redeemedAmount;

        double calculateTaxAmount(double amount, double monthlyTaxPerUnit)
        {
            return amount * monthlyTaxPerUnit;
        }
        double calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments)
        {
            double aux = std::pow(1 + monthlyTaxPerUnit, payments);
            return (amount * monthlyTaxPerUnit * aux) / (aux - 1);
        }
        Amortization calculateAmortization(Amortization& amortization)
        {

        }

    public:

    AmortizationTable(double debtAmount, double anualTax, int initialPayouts)
    {
        _debtAmount = debtAmount;
        _anualTax = anualTax;
        _initialPayouts = initialPayouts;
    }

    int    getPayouts()         const { return table.size();     }
    double getDebtAmount()      const { return _debtAmount;      }
    double getAnualTax()        const { return _anualTax;        }
    double getPayoutsNum()      const { return _initialPayouts;  }
    double getTotalAmount()     const { return _paidAmount;      }
    double getInterestAmount()  const { return _interestAmount;  }
    double getCapitalAmount()   const { return _capitalAmount;   }
    double getRedeemedAmount()  const { return _redeemedAmount;  }

    void GenerateAmortizationTable()
    {
        double debtAmount = _debtAmount;
        double mortageAmount = calculateMortagePayment(debtAmount, _anualTax / 1200.0, _initialPayouts);

        while (debtAmount > 0)
        {
            double taxAmount = calculateTaxAmount(debtAmount, _anualTax / 1200.0);
            Amortization amortization(debtAmount, mortageAmount, taxAmount);
        }
    }

    void modifyAmortization(Amortization& amortization)
    {
        
    }

    friend std::ostream& operator<<(std::ostream& os, const AmortizationTable& at)
    {
        os << "Debt: " << at.getDebtAmount() << " interest: " << at.getAnualTax() << " payouts: " << at.getPayouts() << std::endl;
        os << "Total paid amount: " << at.getTotalAmount() << std::endl;
        os << "Interest amount:   " << at.getInterestAmount() << std::endl;
        os << "Capital amount:    " << at.getCapitalAmount() << std::endl;
        os << "Redeemed amount:   " << at.getRedeemedAmount() << std::endl;
        return os;
    }
};

int main (void)
{
    double amount = 68000.0;
    double anualTaxPercent = 3;
    int years = 25;

    AmortizationTable table(amount, anualTaxPercent, years*12);
    table.GenerateAmortizationTable();
}


