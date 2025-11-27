//LifeLink - Blood Donation Management System

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>

using namespace std;



class Person {
protected:
    string name;
    int age;
    string bloodGroup;
    string contact;

public:

  Person(string n = "", int a = 0, string bg = "", string c = "") 
        : name(n), age(a), bloodGroup(bg), contact(c) {}
    

    virtual ~Person() {}
    


    virtual void display() const {
        cout << "Name: " << name << " | Age: " << age 
             << " | Blood: " << bloodGroup << " | Contact: " << contact;
    }
    
    string getName()
    const { return name; }
    string getBloodGroup()
    const { return bloodGroup; }
    string getContact()
    const { return contact; }
    int getAge()
    const { return age; }
};

class Donor : public Person {
private:
    int donorID;
    string lastDonationDate;
    int totalDonations;
    static int donorCount;

public:

    Donor(string n = "", int a = 0, string bg = "", string c = "", 
          int id = 0, string date = "Never", int donations = 0)
        : Person(n, a, bg, c), donorID(id), lastDonationDate(date), 
          totalDonations(donations)
    {
        donorCount++;
    }
    
    ~Donor() {
        donorCount--;
    }
    
    static int getDonorCount() {
        return donorCount;
    }
    
    void display() {
        cout << "ID: " << donorID << " | ";
        Person::display();
        cout << " | Last Donation: " << lastDonationDate 
             << " | Total: " << totalDonations << endl;
    }
    
   
   
    friend void updateDonation(Donor& donor, string date);


    Donor& operator++() {
        totalDonations++;
        return *this;
    }
    
    bool operator==(const Donor& other) const {
        return donorID == other.donorID;
    }
    
    


    operator int() const {
        return totalDonations;
    }

   


    friend ostream& operator<<(ostream& os, const Donor& donor) {
        os << "ID: " << donor.donorID << " | Name: " << donor.name 
           << " | Age: " << donor.age << " | Blood: " << donor.bloodGroup 
           << " | Contact: " << donor.contact 
           << " | Last Donation: " << donor.lastDonationDate 
           << " | Total: " << donor.totalDonations;
        return os;
    }
    

    int getDonorID() 
    const { return donorID; }
    int getTotalDonations() 
    const { return totalDonations; }
    string getLastDonationDate() 
    const { return lastDonationDate; }
    
    
    void setLastDonationDate(string date) 
    { lastDonationDate = date; }
};


int Donor::donorCount = 0;


void updateDonation(Donor& donor, string date) {
    donor.lastDonationDate = date;

    ++donor; 
}



class Recipient : public Person {
private:
    int recipientID;
    string requestDate;
    int unitsNeeded;
    bool fulfilled;

public:
    
    Recipient(string n = "", int a = 0, string bg = "", string c = "",
              int id = 0, string date = "", int units = 0)
        : Person(n, a, bg, c), recipientID(id), requestDate(date),
          unitsNeeded(units), fulfilled(false) {}
    

    void display(){

        cout << "ID: " << recipientID << " | ";
        Person::display();
        cout << " | Request Date: " << requestDate 
             << " | Units: " << unitsNeeded 
             << " | Status: " << (fulfilled ? "Fulfilled" : "Pending") << endl;
    }



    friend ostream& operator<<(ostream& os, const Recipient& recipient) {
        os << "ID: " << recipient.recipientID << " | Name: " << recipient.name 
           << " | Age: " << recipient.age << " | Blood: " << recipient.bloodGroup 
           << " | Contact: " << recipient.contact 
           << " | Request Date: " << recipient.requestDate 
           << " | Units: " << recipient.unitsNeeded 
           << " | Status: " << (recipient.fulfilled ? "Fulfilled" : "Pending");
        return os;
    }
    
    
    int getRecipientID()
    const { return recipientID; }
    bool isFulfilled() 
    const { return fulfilled; }
    int getUnitsNeeded() 
    const { return unitsNeeded; }
    
    
    void setFulfilled(bool status) { fulfilled = status; }
};





template <typename T>
class BloodBank {
private:
    vector<T*> records;
    
public:

    void addRecord(T* record) {
        records.push_back(record);
    }
    
    vector<T*>& getAllRecords() {
        return records;
    }
    


    vector<T*> findByBloodGroup(string bloodGroup) {
        vector<T*> result;
        for (auto* record : records) {
            if (record->getBloodGroup() == bloodGroup) {
                result.push_back(record);
            }
        }
        return result;
    }
    

    int getCount() const {
        return records.size();
    }
    

    ~BloodBank() {
        for (auto* record : records) {
            delete record;
        }
    }
};





class BloodInventory {
private:
    map<string, int> inventory; 
    
public:

    BloodInventory() {
        inventory["A+"] = 0;
        inventory["A-"] = 0;
        inventory["B+"] = 0;
        inventory["B-"] = 0;
        inventory["AB+"] = 0;
        inventory["AB-"] = 0;
        inventory["O+"] = 0;
        inventory["O-"] = 0;
    }
    

    void addBlood(string bloodGroup, int units) {
        if (inventory.find(bloodGroup) != inventory.end()) {
            inventory[bloodGroup] += units;
        }
    }
    

    bool removeBlood(string bloodGroup, int units) {
        if (inventory.find(bloodGroup) != inventory.end()) {
            if (inventory[bloodGroup] >= units) {
                inventory[bloodGroup] -= units;
                return true;
            }
        }
        return false;
    }
    


    int checkStock(string bloodGroup) {
        if (inventory.find(bloodGroup) != inventory.end()) {
            return inventory[bloodGroup];
        }
        return 0;
    }
    



    void displayInventory() const {
        cout << "\n========== BLOOD INVENTORY ==========\n";
        cout << left << setw(12) << "Blood Group" << "Units Available\n";
        cout << "-------------------------------------\n";
        for (const auto& pair : inventory) {
            cout << left << setw(12) << pair.first << pair.second << endl;
        }
        cout << "=====================================\n";
    }
    
    



    
    BloodInventory operator+(const BloodInventory& other) const {
        BloodInventory result = *this;
        for (const auto& pair : other.inventory) {
            result.inventory[pair.first] += pair.second;
        }
        return result;
    }




    BloodInventory operator-(const BloodInventory& other) const {
        BloodInventory result = *this;
        for (const auto& pair : other.inventory) {

            if (result.inventory.count(pair.first) && result.inventory[pair.first] >= pair.second) {
                result.inventory[pair.first] -= pair.second;
            } 
            
            else if (result.inventory.count(pair.first)) {
                
                result.inventory[pair.first] = 0;
            }
        }
        return result;
    }
    




    const map<string, int>& getInventoryMap() const {
        return inventory;
    }
    

    void setInventoryMap(const map<string, int>& inv) {
        inventory = inv;
    }
};






class FileHandler {
public:

    static void saveDonors(const vector<Donor*>& donors) {
        ofstream file("donors.txt");
        if (file.is_open()) {
            for (const auto* donor : donors) {
                file << donor->getDonorID() << "|" 
                     << donor->getName() << "|" 
                     << donor->getAge() << "|"
                     << donor->getBloodGroup() << "|" 
                     << donor->getContact() << "|"
                     << donor->getLastDonationDate() << "|" 
                     << donor->getTotalDonations() << "\n";
            }
           
            file.close();
            cout << "Donors saved successfully!\n";
        }
    }
    



    static void loadDonors(BloodBank<Donor>& bank) {
        ifstream file("donors.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = 0;
                vector<string> tokens;
                while ((pos = line.find('|')) != string::npos) {
                    tokens.push_back(line.substr(0, pos));
                    line.erase(0, pos + 1);
                }
                tokens.push_back(line);
                
                if (tokens.size() == 7) {
        Donor* donor = new Donor(tokens[1], stoi(tokens[2]), 
                                 tokens[3], tokens[4], 
                                 stoi(tokens[0]), tokens[5], 
                                 stoi(tokens[6]));
                    bank.addRecord(donor);
                }
            }

            file.close();
        }
    }
    



    static void loadRecipients(BloodBank<Recipient>& bank) {
        ifstream file("recipients.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = 0;
                vector<string> tokens;
                while ((pos = line.find('|')) != string::npos) {
                    tokens.push_back(line.substr(0, pos));
                    line.erase(0, pos + 1);
                }
                tokens.push_back(line);
                
                if (tokens.size() == 7) {
 Recipient* recipient = new Recipient(tokens[1], stoi(tokens[2]), 
                                      tokens[3], tokens[4], 
                                      stoi(tokens[0]), "", 
                                      stoi(tokens[5]));
                    recipient->setFulfilled(stoi(tokens[6]));
                    bank.addRecord(recipient);
                }
            }

            file.close();
        }
    }
    



    static void saveRecipients(const vector<Recipient*>& recipients) {
        ofstream file("recipients.txt");
        if (file.is_open()) {
            for (const auto* recipient : recipients) {
                file << recipient->getRecipientID() << "|" 
                     << recipient->getName() << "|"
                     << recipient->getAge() << "|" 
                     << recipient->getBloodGroup() << "|"
                     << recipient->getContact() << "|" 
                     << recipient->getUnitsNeeded() << "|"
                     << recipient->isFulfilled() << "\n";
            }
            file.close();
            cout << "Recipients saved successfully!\n";
        }
    }
    




    static void saveInventory(const map<string, int>& inventory) {
        ofstream file("inventory.txt");
        if (file.is_open()) {
            for (const auto& pair : inventory) {
                file << pair.first << "|" << pair.second << "\n";
            }
            file.close();
        }
    }
    


    static void loadInventory(map<string, int>& inventory) {
        ifstream file("inventory.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos = line.find('|');
                if (pos != string::npos) {
                    string bloodGroup = line.substr(0, pos);
                    int units = stoi(line.substr(pos + 1));
                    inventory[bloodGroup] = units;
                }
            }
            file.close();
        }
    }
};





template <typename T>
void displayAll(const vector<T*>& records) {
    if (records.empty()) {
        cout << "No records found!\n";
        return;
    }


    for (const auto* record : records) {

        cout << *record << endl; 
    }
}



template <typename T>
T* findByID(vector<T*>& records, int id) {
    for (auto* record : records) {
        if constexpr (is_same<T, Donor>::value) {
            if (record->getDonorID() == id) return record;
        } else if constexpr (is_same<T, Recipient>::value) {
            if (record->getRecipientID() == id) return record;
        }
    }
    return nullptr;
}





class LifeLinkApp {
private:
    BloodBank<Donor> donorBank;
    BloodBank<Recipient> recipientBank;
    BloodInventory inventory;
    int nextDonorID;
    int nextRecipientID;
    
public:

    LifeLinkApp() : nextDonorID(1001), nextRecipientID(2001) {
        FileHandler::loadDonors(donorBank);
        FileHandler::loadRecipients(recipientBank);
        
        map<string, int> invMap = inventory.getInventoryMap();
        FileHandler::loadInventory(invMap);
        inventory.setInventoryMap(invMap);
        
        if (donorBank.getCount() > 0) {
            nextDonorID = donorBank.getAllRecords().back()->getDonorID() + 1;
        }
        if (recipientBank.getCount() > 0) {
            nextRecipientID = recipientBank.getAllRecords().back()->getRecipientID() + 1;
        }
    }
    
    

    ~LifeLinkApp() {
    
        
        cout << "\nSaving data before exit...\n";
        FileHandler::saveDonors(donorBank.getAllRecords());
        FileHandler::saveRecipients(recipientBank.getAllRecords());
        FileHandler::saveInventory(inventory.getInventoryMap());
    }
    




    void registerDonor() {
        string name, bloodGroup, contact;
        int age;
        
        cout << "\n===== REGISTER NEW DONOR =====\n";
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        cout << "Age: "; cin >> age;
        cout << "Blood Group (A+/A-/B+/B-/AB+/AB-/O+/O-): "; cin >> bloodGroup;
        cout << "Contact: "; cin >> contact;
        
        Donor* donor = new Donor(name, age, bloodGroup, contact, 
                                 nextDonorID++, "Never", 0);
                
                      donorBank.addRecord(donor);
        
        cout << "\n✓ Donor registered successfully! ID: " 
             << donor->getDonorID() << endl;
    }
    




    void recordDonation() {
        int id;
        cout << "\nEnter Donor ID: ";
        cin >> id;
        
        Donor* donor = findByID(donorBank.getAllRecords(), id);
        if (donor) {
            time_t now = time(0);
            tm* ltm = localtime(&now);
            string date = to_string(ltm->tm_mday) + "/" + 
                          to_string(1 + ltm->tm_mon) + "/" + 
                          to_string(1900 + ltm->tm_year);
            
           
                 updateDonation(*donor, date); 

            inventory.addBlood(donor->getBloodGroup(), 1);
            
            cout << "\n✓ Donation recorded successfully!\n";


            cout << *donor << endl;
        } else {
            cout << "\n✗ Donor not found!\n";
        }
    }
    




    void registerRecipient() {
        string name, bloodGroup, contact, date;
        int age, units;
        
        cout << "\n===== REGISTER RECIPIENT =====\n";
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        cout << "Age: "; cin >> age;
        cout << "Blood Group: "; cin >> bloodGroup;
        cout << "Contact: "; cin >> contact;
        cout << "Units Needed: "; cin >> units;
        
        time_t now = time(0);
        tm* ltm = localtime(&now);
        date = to_string(ltm->tm_mday) + "/" + 
               to_string(1 + ltm->tm_mon) + "/" + 
               to_string(1900 + ltm->tm_year);
        
        Recipient* recipient = new Recipient(name, age, bloodGroup, 
                                             contact, nextRecipientID++, 
                                             date, units);
        recipientBank.addRecord(recipient);
        
        cout << "\n✓ Recipient registered! ID: " 
             << recipient->getRecipientID() << endl;
    }
    




    void fulfillRequest() {
        int id;
        cout << "\nEnter Recipient ID: ";
        cin >> id;        

        Recipient* recipient = findByID(recipientBank.getAllRecords(), id);
        if (recipient && !recipient->isFulfilled()) {
            int available = inventory.checkStock(recipient->getBloodGroup());
            if (available >= recipient->getUnitsNeeded()) {

                inventory.removeBlood(recipient->getBloodGroup(), 
                                      recipient->getUnitsNeeded());
                recipient->setFulfilled(true);

                cout << "\n✓ Request fulfilled successfully!\n";
            } 
            
        else {
                cout << "\n✗ Insufficient blood units! Available: " 
                     << available << endl;
            }
        } 
        
        else if (recipient && recipient->isFulfilled()) {
            cout << "\n✗ Request already fulfilled!\n";
        } 
        
        else {
            cout << "\n✗ Recipient not found!\n";
        }
    }
    




    void viewDonors() {
        cout << "\n========== ALL DONORS ==========\n";
        displayAll(donorBank.getAllRecords());
        cout << "\nTotal Donors: " << Donor::getDonorCount() << endl;
    }
    



    void viewRecipients() {
        cout << "\n========== ALL RECIPIENTS ==========\n";
        displayAll(recipientBank.getAllRecords());
    }
    




    void searchDonorsByBlood() {
        string bloodGroup;
        cout << "\nEnter Blood Group: ";
        cin >> bloodGroup;
        
        vector<Donor*> result = donorBank.findByBloodGroup(bloodGroup);
        cout << "\n========== DONORS WITH " << bloodGroup << " ==========\n";
        displayAll(result);
    }
    




    void run() {
        int choice;
        
        do {
            cout << "\n╔══════════════════════════════════════════╗\n";
            cout <<   "║   LIFELINK - BLOOD DONATION SYSTEM       ║\n";
            cout <<   "╚══════════════════════════════════════════╝\n";
            cout << "1. Register Donor\n";
            cout << "2. Record Donation\n";
            cout << "3. Register Recipient\n";
            cout << "4. Fulfill Blood Request\n";
            cout << "5. View All Donors\n";
            cout << "6. View All Recipients\n";
            cout << "7. Search Donors by Blood Group\n";
            cout << "8. View Blood Inventory\n";
            cout << "9. Exit\n";
            cout << "────────────────────────────────────────\n";
            
            cout << "Enter choice: ";
            cin >> choice;
           

            switch (choice) {
                case 1: registerDonor();
                break;
                case 2: recordDonation();
                break;
                case 3: registerRecipient();
                break;
                case 4: fulfillRequest();
                break;
                case 5: viewDonors();
                break;
                case 6: viewRecipients();
                break;
                case 7: searchDonorsByBlood();
                break;
                case 8: inventory.displayInventory();
                break;
                case 9: cout << "\nThank you for using LifeLink!\n";
                break;

                default: cout << "\n✗ Invalid choice!\n";
            }

        } while (choice != 9);
    }
};





int main() {

    cout<<"\n";
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║         WELCOME TO LIFELINK            ║\n";
    cout << "║    Blood Donation Management System    ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    LifeLinkApp app;
    app.run();
    
    return 0;
}
