#ifndef MORTAGE_HPP
#define MORTAGE_HPP

#include <ostream>
#include <vector>
#include <ctime>

enum RedemptionType
{
    AMOUNT,
    TERM
};

struct Amortization
{
    double paymentAmount;       //Cuota
    double interestAmount;      //Interés
    double amortizationAmount;  //Principal
    double redemptionAmount;    //Amortización anticipada

    double debtAmount;          //Deuda base
    double remainderAmount;     //Deuda pendiente
    
    std::tm* date;

    Amortization();
    Amortization(double debt, double payment, double interest, double redeem = 0);
};

struct Redemption
{
    double redemptionAmount;
    RedemptionType type;
    std::tm* date;

    Redemption();
    Redemption(double amount, RedemptionType redeemType, std::tm& redeemDate);
};

struct Interest
{
    double anualRatePercent;
    double monthRatePerunit;
    std::tm* date;

    Interest();
    Interest(double anualRate);
};


class AmortizationTable
{
    public:
        AmortizationTable();
        AmortizationTable(double debtAmount, double anualTax, int initialPayouts);
        
        void AddRedemption(Redemption redeem);
        void AddRedemption(std::vector<Redemption> redeems);
        void DeleteRedemption(Redemption redeem);
        void DeleteRedemptions();

        void GetRedemptions();
        std::vector<Amortization> GetAmortizationTable();

        void SetInterest(std::vector<Interest> interests);


        int    GetPayouts()         const { return _table.size();    }
        double GetDebtAmount()      const { return _debtAmount;      }
        double GetAnualTax()        const { return _anualTax;        }
        double GetPayoutsNum()      const { return _initialPayouts;  }
        double GetTotalAmount()     const { return _paidAmount;      }
        double GetInterestAmount()  const { return _interestAmount;  }
        double GetCapitalAmount()   const { return _capitalAmount;   }
        double GetRedeemedAmount()  const { return _redeemedAmount;  }

        friend std::ostream& operator<<(std::ostream& os, const AmortizationTable& at)
        {
            os << "Debt: " << at.GetDebtAmount() << " interest: " << at.GetAnualTax() << " payouts: " << at.GetPayouts() << std::endl;
            os << "Total paid amount: " << at.GetTotalAmount() << std::endl;
            os << "Interest amount:   " << at.GetInterestAmount() << std::endl;
            os << "Capital amount:    " << at.GetCapitalAmount() << std::endl;
            os << "Redeemed amount:   " << at.GetRedeemedAmount() << std::endl;
            return os;
        }

    private:
        std::vector<Amortization> _table;
        std::vector<Redemption> _redeems;
        std::vector<Interest> _interest;
        double _debtAmount;
        double _anualTax;
        int _initialPayouts;

        double _paidAmount;
        double _interestAmount;
        double _capitalAmount;
        double _redeemedAmount;

        double calculateTaxAmount(double amount, double monthlyTaxPerUnit);
        double calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments);
};




#endif
