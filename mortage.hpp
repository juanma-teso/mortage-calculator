#ifndef MORTAGE_HPP
#define MORTAGE_HPP

#include <ostream>
#include <iomanip>
#include <vector>
#include <ctime>

enum RedemptionType
{
    REDEEM_TIME,
    REDEEM_AMOUNT
};

struct Date
{
    int year;
    int month;
    Date();
    Date(int year, int month);

    bool operator==(const Date& obj)  {   return year == obj.year && month == obj.month;              }
    bool operator!=(const Date& obj)  {   return !(*this == obj);     }
    bool operator> (const Date& obj)  {   return year * 12 + month >  obj.year * 12 + obj.month;      }
    bool operator>=(const Date& obj)  {   return year * 12 + month >= obj.year * 12 + obj.month;      }
    bool operator< (const Date& obj)  {   return year * 12 + month <  obj.year * 12 + obj.month;      }
    bool operator<=(const Date& obj)  {   return year * 12 + month <= obj.year * 12 + obj.month;      }

    friend Date operator+ (Date lhs, const int& rhs)
    {
        return lhs += rhs;
    }
    Date& operator+=(const int& obj)
    {
        year += (month + obj) / 13;
        month = (month + obj) % 13;
        month = month == 0 ? 1 : month;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Date& date)
    {
        os << std::setw(2) << std::setfill('0') << date.month << "/" << std::setw(4) << date.year << std::setfill(' ');
        return os;
    }
};


struct Amortization
{
    double paymentAmount;       //Cuota
    double interestAmount;      //Interés
    double principalAmount;     //Principal
    double redemptionAmount;    //Amortización anticipada

    double debtAmount;          //Deuda base
    double remainderAmount;     //Deuda pendiente
    
    Date date;

    Amortization();
    Amortization(Date date, double debt, double payment, double interest, double redeem);
};

struct Redemption
{
    double redemptionAmount;
    RedemptionType type;
    Date date;

    Redemption(Date date);
    Redemption(Date date, double amount, RedemptionType redeemType);

    bool operator==(const Redemption& obj)
    {
        return  redemptionAmount == obj.redemptionAmount &&
                type == obj.type &&
                date == obj.date;
    }
    bool operator!=(const Redemption& obj)  {   return !(*this == obj);     }
    bool operator> (const Redemption& obj)  {   return date >  obj.date;    }
    bool operator>=(const Redemption& obj)  {   return date >= obj.date;    }
    bool operator< (const Redemption& obj)  {   return date <  obj.date;    }
    bool operator<=(const Redemption& obj)  {   return date <= obj.date;    }

    friend Redemption operator+ (Redemption lhs, const Redemption& rhs)
    {
        return lhs += rhs;
    }
    friend Redemption operator- (Redemption lhs, const Redemption& rhs)
    {
        return lhs -= rhs;
    }
    Redemption& operator+=(const Redemption& obj)
    {
        redemptionAmount += obj.redemptionAmount;
        return *this;
    }
    Redemption& operator-=(const Redemption& obj)
    {
        redemptionAmount -= obj.redemptionAmount;
        return *this;
    }
};

struct Interest
{
    double anualRatePercent;
    double monthRatePerunit;
    Date date;

    Interest(Date date);
    Interest(Date date, double anualRate);
};

class AmortizationTable
{
    public:
        AmortizationTable(double debtAmount, double anualTax, int initialPayouts);
        void CalculateAmortization();

        void AddRedemption(Redemption redeem);
        void AddRedemption(std::vector<Redemption> redeems);
        void DeleteRedemption(Redemption redeem);
        void DeleteRedemption(Date date);
        void DeleteRedemptions();

        std::vector<Redemption> GetRedemptions();
        std::vector<Amortization> GetAmortizationTable();

        void SetInterest(std::vector<Interest> interests);


        int    GetPayouts()         const { return _table.size();    }
        double GetDebtAmount()      const { return _debtAmount;      }
        double GetAnualInterest()   const { return _anualInterest;   }
        double GetPayoutsNum()      const { return _initialPayouts;  }
        Date   GetStartDate()       const { return _startDate;       }
        double GetTotalAmount()     const { return _paidAmount;      }
        double GetInterestAmount()  const { return _interestAmount;  }
        double GetCapitalAmount()   const { return _principalAmount; }
        double GetRedeemedAmount()  const { return _redeemedAmount;  }

        friend std::ostream& operator<<(std::ostream& os, const AmortizationTable& at)
        {
            os << "Debt: " << at.GetDebtAmount() << " interest: " << at.GetAnualInterest() << " payouts: " << at.GetPayouts() << std::endl;
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
        double _anualInterest;
        int _initialPayouts;

        double _paidAmount;
        double _interestAmount;
        double _principalAmount;
        double _redeemedAmount;
        int _finalPayouts;
        Date _startDate;

        double calculateRedemption(Redemption& redemption);
        double calculateTaxAmount(Interest& interest);
        double calculateMortagePayment(double amount, double monthlyTaxPerUnit, double payments);
        Redemption getRedemption(Date* date);
        Interest getInterest(Date* date);
};

#endif
