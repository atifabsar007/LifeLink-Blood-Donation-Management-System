// LifeLink - Blood Donation Management System (Console C++ - Single File)
// Features: Donor registration, eligibility check, blood inventory (8 groups),
// emergency requests with priority, auto matching, donation history, camps,
// certificate generation (text file), simple reports, file-based persistence.
// Build with: g++ -std=c++17 LifeLink_main.cpp -o lifelink

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <map>        // For std::map
#include <iomanip>    // For formatting output

using namespace std;

// ----------------------------- Utilities ---------------------------------
static const vector<string> BLOOD_GROUPS = {"A+","A-","B+","B-","AB+","AB-","O+","O-"};

string todayDate() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return string(buf);
}

// parse date YYYY-MM-DD to time_t (approx)
time_t parseDate(const string &d) {
    tm tm_ = {};
    if (sscanf(d.c_str(), "%d-%d-%d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday) != 3) return 0;
    tm_.tm_year -= 1900;
    tm_.tm_mon -= 1;
    tm_.tm_hour = 12;
    return mktime(&tm_);
}

int daysBetween(const string &a, const string &b) {
    time_t ta = parseDate(a);
    time_t tb = parseDate(b);
    double diff = difftime(tb, ta);
    return static_cast<int>(diff / (60*60*24));
}

bool validBloodGroup(const string &bg) {
    return find(BLOOD_GROUPS.begin(), BLOOD_GROUPS.end(), bg) != BLOOD_GROUPS.end();
}

// ----------------------------- Classes ----------------------------------

class Person {
public:
    string id;
    string name;
    string contact;
    string address;
    Person() {}
    Person(string id_, string name_, string contact_, string address_): id(id_), name(name_), contact(contact_), address(address_) {}
    virtual ~Person() {}
    virtual void print() const = 0;
};

class Donor : public Person {
public:
    string bloodGroup;
    int age;
    double weight;
    string lastDonation; // YYYY-MM-DD
    int totalDonations;

    Donor() { totalDonations = 0; }
    Donor(string id_, string name_, string contact_, string address_, string bg, int age_, double weight_, string lastDonation_, int totalDonations_)
    : Person(id_,name_,contact_,address_), bloodGroup(bg), age(age_), weight(weight_), lastDonation(lastDonation_), totalDonations(totalDonations_) {}

    bool isEligible() const {
        // Age min 18, max 65; weight >=50; gap >=90 days
        if (age < 18 || age > 65) return false;
        if (weight < 50.0) return false;
        if (!lastDonation.empty()) {
            int gap = daysBetween(lastDonation, todayDate());
            if (gap < 90) return false;
        }
        return true;
    }

    string nextEligibleDate() const {
        if (lastDonation.empty()) return todayDate();
        time_t t = parseDate(lastDonation);
        t += 90 * 24 * 60 * 60; // add 90 days
        tm *lt = localtime(&t);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
        return string(buf);
    }

    void print() const override {
        cout << id << " | " << name << " | " << bloodGroup << " | Age:" << age << " | Wt:" << weight << " | Last:" << lastDonation << " | Total:" << totalDonations << "\n";
    }
};

class BloodUnit {
public:
    string group;
    string collectedDate;
    string expiryDate; // collectedDate + 42 days
    string donorID; // optional
    BloodUnit() {}
    BloodUnit(string g, string c, string e, string d): group(g), collectedDate(c), expiryDate(e), donorID(d) {}
};

class BloodInventory {
public:
    // store units as vector of BloodUnit to handle expiry
    vector<BloodUnit> units;

    void addUnit(const string &group, const string &collectedDate, const string &donorID="") {
        // expiry = collected + 42 days
        time_t t = parseDate(collectedDate);
        t += 42LL * 24 * 60 * 60;
        tm *lt = localtime(&t);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
        units.emplace_back(group, collectedDate, string(buf), donorID);
    }

    int countGroup(const string &group) const {
        int c = 0;
        string td = todayDate();
        for (auto &u: units) if (u.group==group && daysBetween(u.collectedDate, td) <= 42) c++;
        return c;
    }

    // remove oldest units first, return actually removed number
    int removeUnits(const string &group, int need) {
        sort(units.begin(), units.end(), [](const BloodUnit &a, const BloodUnit &b){
            return parseDate(a.collectedDate) < parseDate(b.collectedDate);
        });
        int removed = 0;
        string td = todayDate();
        for (auto it = units.begin(); it != units.end() && removed < need;) {
            if (it->group==group && daysBetween(it->collectedDate, td) <= 42) {
                it = units.erase(it);
                removed++;
            } else ++it;
        }
        return removed;
    }

    void removeExpired() {
        string td = todayDate();
        units.erase(remove_if(units.begin(), units.end(), [&](const BloodUnit &u){
            return daysBetween(u.collectedDate, td) > 42;
        }), units.end());
    }

    map<string,int> summary() {
        removeExpired();
        map<string,int> m;
        for (auto &bg: BLOOD_GROUPS) m[bg]=0;
        for (auto &u: units) m[u.group]++;
        return m;
    }
};

enum RequestPriority { NORMAL=3, URGENT=2, CRITICAL=1 };

class BloodRequest {
public:
    string id;
    string patientName;
    string bloodGroup;
    int unitsNeeded;
    RequestPriority priority;
    string requestDate;
    string status; // Pending, Matched, Fulfilled, Cancelled
    string matchedDonorID;
    string fulfilledDate;
    BloodRequest() {}
    BloodRequest(string id_, string patientName_, string bg_, int units_, RequestPriority p_, string requestDate_)
    : id(id_), patientName(patientName_), bloodGroup(bg_), unitsNeeded(units_), priority(p_), requestDate(requestDate_), status("Pending") {}
};

class DonationCamp {
public:
    string id;
    string date;
    string location;
    string organizer;
    vector<string> registeredDonors; // donor IDs
    int unitsCollected;
    DonationCamp() : unitsCollected(0) {}
};

// --------------------------- BloodBank System ----------------------------

class BloodBank {
public:
    vector<Donor> donors;
    vector<BloodRequest> requests;
    BloodInventory inventory;
    vector<DonationCamp> camps;

    // persistence filenames
    const string donorsFile = "donors.csv";
    const string inventoryFile = "inventory.csv"; // one unit per row
    const string requestsFile = "requests.csv";
    const string campsFile = "camps.csv";

    BloodBank() {
        loadAll();
    }

    ~BloodBank() {
        saveAll();
    }

    // ---------- ID generation helpers
    string nextDonorID() {
        int maxid = 0;
        for (auto &d: donors) {
            if (d.id.size()>1 && d.id[0]=='D') {
                int v = stoi(d.id.substr(1));
                if (v>maxid) maxid=v;
            }
        }
        return "D" + to_string(maxid+1);
    }
    string nextRequestID() {
        int maxid = 0;
        for (auto &r: requests) {
            if (r.id.size()>1 && r.id[0]=='R') {
                int v = stoi(r.id.substr(1)); if (v>maxid) maxid=v;
            }
        }
        return "R" + to_string(maxid+1);
    }
    string nextCampID() {
        int maxid = 0;
        for (auto &c: camps) {
            if (c.id.size()>1 && c.id[0]=='C') {
                int v = stoi(c.id.substr(1)); if (v>maxid) maxid=v;
            }
        }
        return "C" + to_string(maxid+1);
    }

    // ---------------- Donor operations ----------------
    void registerDonor() {
        string name, contact, address, bg, lastDonation;
        int age; double weight;
        cout << "Enter name: "; getline(cin, name);
        cout << "Enter age: "; cin >> age; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter weight (kg): "; cin >> weight; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter blood group (e.g., A+): "; getline(cin, bg);
        if (!validBloodGroup(bg)) { cout<<"Invalid blood group. Aborting.\n"; return; }
        cout << "Enter contact: "; getline(cin, contact);
        cout << "Enter address: "; getline(cin, address);
        cout << "Last donation date (YYYY-MM-DD) or blank: "; getline(cin, lastDonation);
        string id = nextDonorID();
        Donor d(id,name,contact,address,bg,age,weight,lastDonation,0);
        donors.push_back(d);
        cout << "Donor registered with ID: " << id << "\n";
        saveDonors();
    }

    void listDonors() {
        cout << "-- Donors (" << donors.size() << ") --\n";
        for (auto &d: donors) d.print();
    }

    Donor* findDonorByID(const string &id) {
        for (auto &d: donors) if (d.id==id) return &d;
        return nullptr;
    }

    vector<Donor*> searchDonorsByGroup(const string &bg) {
        vector<Donor*> res;
        for (auto &d: donors) if (d.bloodGroup==bg && d.isEligible()) res.push_back(&d);
        return res;
    }

    // donation action: updates donor, adds inventory
    void processDonation() {
        string donorID;
        cout << "Enter Donor ID: "; getline(cin, donorID);
        Donor* d = findDonorByID(donorID);
        if (!d) { cout<<"Donor not found.\n"; return; }
        if (!d->isEligible()) {
            cout << "Donor not eligible. Next eligible: " << d->nextEligibleDate() << "\n"; return;
        }
        // simulate one unit = 1
        string today = todayDate();
        inventory.addUnit(d->bloodGroup, today, d->id);
        d->lastDonation = today;
        d->totalDonations += 1;
        cout << "Donation recorded. 1 unit added to inventory.\n";
        generateCertificate(*d);
        saveDonors(); saveInventory();
    }

    void generateCertificate(const Donor &d) {
        string fname = d.id + "_certificate.txt";
        ofstream f(fname);
        f << "--- Donor Appreciation Certificate ---\n";
        f << "Donor ID: " << d.id << "\n";
        f << "Name: " << d.name << "\n";
        f << "Blood Group: " << d.bloodGroup << "\n";
        f << "Date: " << todayDate() << "\n";
        f << "Thank you for your life-saving donation!\n";
        f.close();
        cout << "Certificate generated: " << fname << "\n";
    }

    // ---------------- Inventory operations ----------------
    void showInventory() {
        inventory.removeExpired();
        auto s = inventory.summary();
        cout << "-- Inventory Summary --\n";
        for (auto &bg: BLOOD_GROUPS) {
            cout << bg << " : " << s[bg];
            if (s[bg] < 5) cout << "  <-- LOW";
            cout << "\n";
        }
    }

    void addInventoryFromDonation() {
        // developer helper: add by specifying group
        string bg; cout << "Blood group: "; getline(cin,bg);
        if (!validBloodGroup(bg)) { cout<<"Invalid bg\n"; return; }
        inventory.addUnit(bg, todayDate());
        saveInventory();
        cout << "1 unit added to inventory for " << bg << "\n";
    }

    // ---------------- Requests ----------------
    void requestBlood() {
        string pname, bg; int units; int prio;
        cout << "Patient name: "; getline(cin, pname);
        cout << "Required blood group: "; getline(cin, bg);
        if (!validBloodGroup(bg)) { cout<<"Invalid blood group.\n"; return; }
        cout << "Units needed: "; cin >> units; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Priority: 1-Critical, 2-Urgent, 3-Normal: "; cin >> prio; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        RequestPriority p = NORMAL;
        if (prio==1) p=CRITICAL; else if (prio==2) p=URGENT; else p=NORMAL;
        string id = nextRequestID();
        BloodRequest r(id, pname, bg, units, p, todayDate());
        requests.push_back(r);
        cout << "Request created: " << id << "\n";
        saveRequests();
        // try to auto-match
        matchRequests();
    }

    // compatibility map
    bool compatible(const string &donorBG, const string &recipientBG) {
        // simplified: use standard rules
        if (recipientBG=="AB+") return true;
        if (donorBG=="O-") return true;
        map<string, vector<string>> canDonate{
            {"O-", {"A+","A-","B+","B-","AB+","AB-","O+","O-"}},
            {"O+", {"O+","A+","B+","AB+"}},
            {"A-", {"A+","A-","AB+","AB-"}},
            {"A+", {"A+","AB+"}},
            {"B-", {"B+","B-","AB+","AB-"}},
            {"B+", {"B+","AB+"}},
            {"AB-", {"AB+","AB-"}},
            {"AB+", {"AB+"}}
        };
        auto it = canDonate.find(donorBG);
        if (it==canDonate.end()) return false;
        return find(it->second.begin(), it->second.end(), recipientBG) != it->second.end();
    }

    // Try to match all pending requests (simple greedy)
    void matchRequests() {
        // process in order of priority and requestDate
        sort(requests.begin(), requests.end(), [](const BloodRequest &a, const BloodRequest &b){
            if (a.priority!=b.priority) return a.priority < b.priority; // CRITICAL first
            return a.requestDate < b.requestDate;
        });
        for (auto &r: requests) {
            if (r.status!="Pending") continue;
            // check inventory first for exact group
            inventory.removeExpired();
            int avail = inventory.countGroup(r.bloodGroup);
            if (avail >= r.unitsNeeded) {
                inventory.removeUnits(r.bloodGroup, r.unitsNeeded);
                r.status = "Fulfilled";
                r.fulfilledDate = todayDate();
                cout << "Request " << r.id << " fulfilled from inventory.\n";
                saveInventory(); saveRequests();
                continue;
            }
            // else try to find donors in system
            vector<Donor*> possible;
            for (auto &d: donors) {
                if (d.isEligible() && compatible(d.bloodGroup, r.bloodGroup)) possible.push_back(&d);
            }
            if (!possible.empty()) {
                // pick nearest by simple heuristic: earliest lastDonation (most ready)
                sort(possible.begin(), possible.end(), [](Donor* a, Donor* b){
                    return a->lastDonation < b->lastDonation;
                });
                // use donors until units satisfied
                int needed = r.unitsNeeded;
                for (auto pd: possible) {
                    if (needed==0) break;
                    // simulate one unit per donor
                    inventory.addUnit(pd->bloodGroup, todayDate(), pd->id);
                    pd->lastDonation = todayDate(); pd->totalDonations++;
                    needed--; cout << "Notified donor " << pd->id << " ("<<pd->name<<") for request "<<r.id<<"\n";
                }
                if (needed==0) {
                    inventory.removeUnits(r.bloodGroup, r.unitsNeeded);
                    r.status = "Fulfilled";
                    r.fulfilledDate = todayDate();
                    cout << "Request "<<r.id<<" fulfilled via matched donors.\n";
                    saveDonors(); saveInventory(); saveRequests();
                } else {
                    cout << "Partial donors found for "<<r.id<<". Still pending.\n";
                    saveDonors(); saveInventory(); saveRequests();
                }
            } else {
                cout << "No eligible donors currently for request "<<r.id<<".\n";
            }
        }
    }

    void listRequests() {
        cout << "-- Requests --\n";
        for (auto &r: requests) {
            cout << r.id << " | " << r.patientName << " | " << r.bloodGroup << " | x" << r.unitsNeeded << " | " << r.requestDate << " | " << r.status << "\n";
        }
    }

    // ---------------- Camps ----------------
    void createCamp() {
        string date, location, organizer;
        cout << "Camp date (YYYY-MM-DD): "; getline(cin, date);
        cout << "Location: "; getline(cin, location);
        cout << "Organizer: "; getline(cin, organizer);
        DonationCamp c; c.id = nextCampID(); c.date=date; c.location=location; c.organizer=organizer; c.unitsCollected=0;
        camps.push_back(c);
        cout << "Camp created: " << c.id << " at " << location << " on " << date << "\n";
        saveCamps();
    }

    void registerDonorToCamp() {
        string campID, donorID; cout << "Camp ID: "; getline(cin, campID);
        auto it = find_if(camps.begin(), camps.end(), [&](const DonationCamp &c){ return c.id==campID; });
        if (it==camps.end()) { cout<<"Camp not found\n"; return; }
        cout << "Donor ID: "; getline(cin, donorID);
        Donor* d = findDonorByID(donorID);
        if (!d) { cout<<"Donor not found\n"; return; }
        it->registeredDonors.push_back(donorID);
        cout << "Donor registered to camp.\n";
        saveCamps();
    }

    // ---------------- Reports ----------------
    void showReports() {
        cout << "--- Reports ---\n";
        // most active donors
        vector<pair<int,string>> ranking;
        for (auto &d: donors) ranking.push_back({d.totalDonations, d.id+":"+d.name});
        sort(ranking.rbegin(), ranking.rend());
        cout << "Top donors:\n";
        for (int i=0;i<min((int)ranking.size(),5);++i) cout << ranking[i].second << " -> " << ranking[i].first << " donations\n";
        // blood distribution
        auto s = inventory.summary();
        cout << "Blood distribution:\n";
        for (auto &bg: BLOOD_GROUPS) cout << bg << ": " << s[bg] << "\n";
        // emergency fulfillment
        int total = 0, fulfilled=0;
        for (auto &r: requests) { total++; if (r.status=="Fulfilled") fulfilled++; }
        cout << "Requests fulfilled: " << fulfilled << "/" << total << "\n";
    }

    // ---------------- Persistence ----------------
    void saveDonors() {
        ofstream f(donorsFile);
        f << "id,name,age,weight,bloodGroup,contact,address,lastDonation,totalDonations\n";
        for (auto &d: donors) {
            f << d.id << "," << d.name << "," << d.age << "," << d.weight << "," << d.bloodGroup << "," << d.contact << "," << d.address << "," << d.lastDonation << "," << d.totalDonations << "\n";
        }
    }
    void loadDonors() {
        donors.clear();
        ifstream f(donorsFile);
        if (!f) return;
        string line; getline(f,line); // header
        while (getline(f,line)) {
            stringstream ss(line);
            string id,name,age_s,weight_s,bg,contact,address,lastDonation,totalDonations_s;
            getline(ss,id,','); getline(ss,name,','); getline(ss,age_s,','); getline(ss,weight_s,','); getline(ss,bg,','); getline(ss,contact,','); getline(ss,address,','); getline(ss,lastDonation,','); getline(ss,totalDonations_s,',');
            Donor d;
            d.id=id; d.name=name; d.age = age_s.empty()?0:stoi(age_s); d.weight = weight_s.empty()?0:stod(weight_s); d.bloodGroup=bg; d.contact=contact; d.address=address; d.lastDonation=lastDonation; d.totalDonations = totalDonations_s.empty()?0:stoi(totalDonations_s);
            donors.push_back(d);
        }
    }

    void saveInventory() {
        ofstream f(inventoryFile);
        f << "group,collectedDate,expiryDate,donorID\n";
        for (auto &u: inventory.units) f << u.group << "," << u.collectedDate << "," << u.expiryDate << "," << u.donorID << "\n";
    }
    void loadInventory() {
        inventory.units.clear();
        ifstream f(inventoryFile);
        if (!f) return;
        string line; getline(f,line);
        while (getline(f,line)) {
            stringstream ss(line);
            string g,c,e,did;
            getline(ss,g,','); getline(ss,c,','); getline(ss,e,','); getline(ss,did,',');
            if (!g.empty()) inventory.units.emplace_back(g,c,e,did);
        }
    }

    void saveRequests() {
        ofstream f(requestsFile);
        f << "id,patientName,bloodGroup,units,priority,requestDate,status,matchedDonorID,fulfilledDate\n";
        for (auto &r: requests) {
            f << r.id << "," << r.patientName << "," << r.bloodGroup << "," << r.unitsNeeded << "," << (int)r.priority << "," << r.requestDate << "," << r.status << "," << r.matchedDonorID << "," << r.fulfilledDate << "\n";
        }
    }
    void loadRequests() {
        requests.clear();
        ifstream f(requestsFile);
        if (!f) return;
        string line; getline(f,line);
        while (getline(f,line)) {
            stringstream ss(line);
            string id,patient,bg,units_s,priority_s,reqDate,status,matched,fulfilled;
            getline(ss,id,','); getline(ss,patient,','); getline(ss,bg,','); getline(ss,units_s,','); getline(ss,priority_s,','); getline(ss,reqDate,','); getline(ss,status,','); getline(ss,matched,','); getline(ss,fulfilled,',');
            BloodRequest r;
            r.id=id; r.patientName=patient; r.bloodGroup=bg; r.unitsNeeded = units_s.empty()?0:stoi(units_s); r.priority = (RequestPriority)(priority_s.empty()?3:stoi(priority_s)); r.requestDate=reqDate; r.status=status; r.matchedDonorID=matched; r.fulfilledDate=fulfilled;
            requests.push_back(r);
        }
    }

    void saveCamps() {
        ofstream f(campsFile);
        f << "id,date,location,organizer,unitsCollected,registeredDonors\n";
        for (auto &c: camps) {
            f << c.id << "," << c.date << "," << c.location << "," << c.organizer << "," << c.unitsCollected << ",";
            for (size_t i=0;i<c.registeredDonors.size();++i) { f<<c.registeredDonors[i]; if (i+1<c.registeredDonors.size()) f<<";"; }
            f << "\n";
        }
    }
    void loadCamps() {
        camps.clear();
        ifstream f(campsFile);
        if (!f) return;
        string line; getline(f,line);
        while (getline(f,line)) {
            stringstream ss(line);
            string id,date,location,organizer,units_s,regs;
            getline(ss,id,','); getline(ss,date,','); getline(ss,location,','); getline(ss,organizer,','); getline(ss,units_s,','); getline(ss,regs,',');
            DonationCamp c; c.id=id; c.date=date; c.location=location; c.organizer=organizer; c.unitsCollected = units_s.empty()?0:stoi(units_s);
            if (!regs.empty()) {
                stringstream rreg(regs);
                string token;
                while (getline(rreg, token, ';')) c.registeredDonors.push_back(token);
            }
            camps.push_back(c);
        }
    }

    void saveAll() { saveDonors(); saveInventory(); saveRequests(); saveCamps(); }
    void loadAll() { loadDonors(); loadInventory(); loadRequests(); loadCamps(); }

    // ---------------- UI menu ----------------
    void mainMenu() {
        while (true) {
            cout << "\n===== LifeLink Blood Bank =====\n";
            cout << "1. Donor Module\n2. Recipient Module\n3. Blood Bank Admin\n4. Reports & Statistics\n5. Donation Camps\n6. Exit\n";
            cout << "Choose: ";
            int ch; cin >> ch; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (ch==1) donorMenu();
            else if (ch==2) recipientMenu();
            else if (ch==3) adminMenu();
            else if (ch==4) showReports();
            else if (ch==5) campsMenu();
            else if (ch==6) { cout<<"Goodbye\n"; break; }
            else cout<<"Invalid\n";
        }
    }

    void donorMenu() {
        while (true) {
            cout << "\n== Donor Module ==\n1.Register New Donor\n2.Check Eligibility\n3.Donate Blood\n4.View Donation History\n5.Update Profile\n6.Search Donors\n7.Back\nChoose: ";
            int ch; cin >> ch; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (ch==1) registerDonor();
            else if (ch==2) { string id; cout<<"Donor ID: "; getline(cin,id); Donor* d=findDonorByID(id); if (!d) cout<<"Not found\n"; else cout<<(d->isEligible()?"Eligible\n":"Not eligible\n"); }
            else if (ch==3) processDonation();
            else if (ch==4) { listDonors(); }
            else if (ch==5) { cout<<"Update not implemented in demo. Use CSV to edit.\n"; }
            else if (ch==6) { string bg; cout<<"Blood group: "; getline(cin,bg); auto res = searchDonorsByGroup(bg); for (auto p: res) p->print(); }
            else if (ch==7) break; else cout<<"Invalid\n";
        }
    }

    void recipientMenu() {
        while (true) {
            cout << "\n== Recipient Module ==\n1.Request Blood\n2.Check Request Status\n3.View Available Blood Groups\n4.Emergency Request\n5.Back\nChoose: ";
            int ch; cin >> ch; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (ch==1) requestBlood();
            else if (ch==2) { listRequests(); }
            else if (ch==3) showInventory();
            else if (ch==4) { cout<<"Emergency request uses high priority during creation.\n"; requestBlood(); }
            else if (ch==5) break; else cout<<"Invalid\n";
        }
    }

    void adminMenu() {
        while (true) {
            cout << "\n== Admin ==\n1.View Inventory\n2.Update Stock\n3.Match Requests\n4.View All Donors\n5.View All Requests\n6.Generate Reports\n7.Low Stock Alerts\n8.Back\nChoose: ";
            int ch; cin >> ch; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (ch==1) showInventory();
            else if (ch==2) addInventoryFromDonation();
            else if (ch==3) matchRequests();
            else if (ch==4) listDonors();
            else if (ch==5) listRequests();
            else if (ch==6) showReports();
            else if (ch==7) { auto s = inventory.summary(); for (auto &bg: BLOOD_GROUPS) if (s[bg] < 5) cout<<bg<<" low: "<<s[bg]<<"\n"; }
            else if (ch==8) break; else cout<<"Invalid\n";
        }
    }

    void campsMenu() {
        while (true) {
            cout << "\n== Camps ==\n1.Create Camp\n2.Register Donor to Camp\n3.View Camps\n4.Back\nChoose: ";
            int ch; cin >> ch; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (ch==1) createCamp();
            else if (ch==2) registerDonorToCamp();
            else if (ch==3) { for (auto &c: camps) cout<<c.id<<" | "<<c.date<<" | "<<c.location<<" | regs:"<<c.registeredDonors.size()<<" units:"<<c.unitsCollected<<"\n"; }
            else if (ch==4) break; else cout<<"Invalid\n";
        }
    }
};

// --------------------------- main --------------------------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    BloodBank bank;
    cout << "LifeLink - Blood Donation Management System\n";
    bank.mainMenu();
    return 0;
}
