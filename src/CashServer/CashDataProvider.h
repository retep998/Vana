/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include "Types.h"
#include <boost/noncopyable.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <string>

using std::tr1::unordered_map;
using std::string;
using std::vector;

struct CashItemInfo {
	CashItemInfo() : gender(-1),
		quantity(0),
		serial(0),
		itemid(0),
		price(0),
		expiration_days(0)
	{ }
	int8_t gender;
	int16_t quantity;
	int16_t expiration_days;
	int32_t serial;
	int32_t itemid;
	int32_t price;
};

struct CouponInfo {
	bool used;
	int32_t maplePoints;
	int32_t nxCredit;
	int32_t nxPrepaid;
	int32_t mesos;
	vector<CashItemInfo> items;
};

struct ModifiedCashItem {
	ModifiedCashItem() : mark(0), flags(0x00), serial(0), discountPrice(0), showUp(true) { }
	int8_t mark;
	int32_t flags;
	int32_t serial;
	int32_t discountPrice;
	bool showUp;
	vector<int32_t> items;
};

class CashDataProvider : boost::noncopyable {
public:
	static CashDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new CashDataProvider();
		return singleton;
	}
	void loadData();

	bool itemExists(int32_t serial) { return cashItems.find(serial) != cashItems.end(); }
	bool packageExists(int32_t packageId) { return cashPackages.find(packageId) != cashPackages.end(); }
	bool couponExists(string code) { return coupons.find(code) != coupons.end(); }
	bool modifiedItemExists(int32_t serial) { return modifiedItemsBySerial.find(serial) != modifiedItemsBySerial.end(); }
	CashItemInfo * getItemInfo(int32_t serial) { return (itemExists(serial) ? &cashItems[serial] : nullptr); }
	vector<CashItemInfo> * getPackageItems(int32_t packageId) { return &cashPackages[packageId]; }
	CouponInfo * getCouponInfo(string code) { return (couponExists(code) ? &coupons[code] : nullptr); }
	ModifiedCashItem * getModifiedCashItemData(int32_t serial) { return (modifiedItemExists(serial) ? &modifiedItemsBySerial[serial] : nullptr); }

	vector<int32_t> * getLimitSellData() { return &limitSell; }
	vector<ModifiedCashItem> * getModifiedItems() { return &modifiedItems; }
	vector<int32_t> * getBestItems() { return &bestItems; }

	
	void updateCoupon(const string &code, bool used);
	void logBoughtItem(int32_t userid, int32_t charid, int32_t packageid);
	void loadBestItems();
private:
	CashDataProvider() {}
	static CashDataProvider *singleton;

	void loadCashItems();
	void loadPackages();
	void loadCoupons();
	void loadCouponItems();
	void loadLimitSell();
	void loadModifiedItems();

	unordered_map<int32_t, CashItemInfo> cashItems;
	unordered_map<int32_t, vector<CashItemInfo> > cashPackages;
	unordered_map<int32_t, ModifiedCashItem> modifiedItemsBySerial;
	unordered_map<string, CouponInfo> coupons;
	vector<int32_t> limitSell;
	vector<ModifiedCashItem> modifiedItems;
	vector<int32_t> bestItems; // The client determines the order >_<
};
