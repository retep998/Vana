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
#include "CashDataProvider.h"
#include "CashServer.h"
#include "Database.h"
#include "GameConstants.h"
#include "InitializeCashServer.h"
#include "InitializeCommon.h"
#include "ItemConstants.h"
#include "ItemDataProvider.h"
#include "StringUtilities.h"

using Initializing::outputWidth;

CashDataProvider * CashDataProvider::singleton = nullptr;

void CashDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Cash data... ";

	loadCashItems();
	loadPackages();
	loadCoupons();
	loadCouponItems();
	loadLimitSell();
	loadModifiedItems();
	loadBestItems();

	std::cout << "DONE" << std::endl;
}

void CashDataProvider::loadCashItems() {
	cashItems.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM cash_commodity_data");
	mysqlpp::UseQueryResult res = query.use();
	CashItemInfo item;

	enum CashData {
		SerialNumber = 0,
		Itemid, Quantity, Price, ExpirationDays, Priority,
		Gender, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		item = CashItemInfo();
		item.serial = atoi(row[SerialNumber]);
		item.itemid = atoi(row[Itemid]);
		item.quantity = atoi(row[Quantity]);
		item.price = atoi(row[Price]);
		item.expiration_days = atoi(row[ExpirationDays]);
		if (row[Gender] == "both")
			item.gender = -1;
		else if (row[Gender] == "male")
			item.gender = 0;
		else if (row[Gender] == "female")
			item.gender = 1;

		cashItems[item.serial] = item;
	}
}

void CashDataProvider::loadPackages() {
	cashPackages.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM cash_package_data ORDER BY packageid ASC");
	mysqlpp::UseQueryResult res = query.use();

	enum PackageData {
		Id = 0,
		PackageId, SerialNumber
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		if (itemExists(atoi(row[SerialNumber]))) {
			cashPackages[atoi(row[PackageId])].push_back(cashItems[atoi(row[SerialNumber])]);
		}
	}
}

void CashDataProvider::loadCoupons() {
	coupons.clear();
	mysqlpp::Query query = Database::getCharDB().query("SELECT * FROM cashshop_coupon_codes");
	mysqlpp::UseQueryResult res = query.use();
	CouponInfo coupon;

	enum CouponData {
		Serial = 0, 
		MaplePoints, NxCredit, NxPrepaid, Mesos, Used
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		coupon = CouponInfo();

		coupon.maplePoints = atoi(row[MaplePoints]);
		coupon.nxCredit = atoi(row[NxCredit]);
		coupon.nxPrepaid = atoi(row[NxPrepaid]);
		coupon.mesos = atoi(row[Mesos]);
		coupon.used = StringUtilities::atob(row[Used]);
		coupons[row[Serial]] = coupon;
	}
}

void CashDataProvider::loadCouponItems() {
	mysqlpp::Query query = Database::getCharDB().query("SELECT * FROM cashshop_coupon_item_rewards");
	mysqlpp::UseQueryResult res = query.use();
	CashItemInfo item;

	enum CouponData {
		Serial = 0, 
		ItemId, Amount, DaysUsable
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		if (!couponExists(row[Serial])) {
			std::stringstream str;
			str << "Coupon '" << row[Serial] << "' doesn't exist for coupon item reward.";
			CashServer::Instance()->log(LogTypes::Warning, str.str());
			continue;
		}

		if (!ItemDataProvider::Instance()->itemExists(atoi(row[ItemId]))) {
			std::stringstream str;
			str << "Coupon '" << row[Serial] << "' has an invalid item id as reward: " << atoi(row[ItemId]) << ". Ignoring.";
			CashServer::Instance()->log(LogTypes::Warning, str.str());
			continue;
		}

		CashItemInfo item;
		item.itemid = atoi(row[ItemId]);
		item.quantity = atoi(row[Amount]);
		item.expiration_days = atoi(row[DaysUsable]);

		getCouponInfo(row[Serial])->items.push_back(item);
	}
}

void CashDataProvider::loadLimitSell() {
	limitSell.clear();
	mysqlpp::Query query = Database::getCharDB().query("SELECT * FROM cashshop_limit_sell");
	mysqlpp::UseQueryResult res = query.use();

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		limitSell.push_back(atoi(row[0]));
	}
}

void CashDataProvider::loadModifiedItems() {
	modifiedItemsBySerial.clear();
	modifiedItems.clear();
	mysqlpp::Query query = Database::getCharDB().query("SELECT * FROM cashshop_modified_items");
	mysqlpp::UseQueryResult res = query.use();
	string tempMark;
	ModifiedCashItem item;

	enum TableData {
		Serial = 0, 
		DiscountPrice, Mark, Showup
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		tempMark = row[Mark];

		item = ModifiedCashItem();
		item.serial = atoi(row[Serial]);
		if (row[DiscountPrice]) {
			item.discountPrice = atoi(row[DiscountPrice]);
			item.flags |= CashItems::Flags::HasDiscount;
		}
		if (row[Mark]) {
			if (tempMark == "event") item.mark = 0x03;
			else if (tempMark == "hot") item.mark = 0x02;
			else if (tempMark == "sale") item.mark = 0x01;
			else if (tempMark == "new") item.mark = 0x00;
			item.flags |= CashItems::Flags::ReplaceMark;
		}
		if (row[Showup]) {
			item.showUp = StringUtilities::atob(row[Showup]);
			item.flags |= CashItems::Flags::RemoveAddItem;
		}

		modifiedItems.push_back(item);
		modifiedItemsBySerial[item.serial] = item;
	}
}

void CashDataProvider::loadBestItems() {
	bestItems.clear();
	mysqlpp::Query query = Database::getCharDB().query("SELECT packageid, COUNT(packageid) AS NumOccurrences FROM cashshop_sell_log GROUP BY packageid ORDER BY NumOccurrences DESC LIMIT 5");
	mysqlpp::UseQueryResult res = query.use();
	int8_t i = 0;

	enum TableData {
		PackageId = 0,
		Amount
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		bestItems.push_back(atoi(row[PackageId]));
		i++;
	}

	for (; i <= 5; i++) {
		bestItems.push_back(0); // We don't want to crash our server.
	}

}

void CashDataProvider::updateCoupon(const string &code, bool used) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE cashshop_coupon_codes SET used = " << (used ? 1 : 0) << " WHERE serial = " << mysqlpp::quote << code << " LIMIT 1";
	query.exec();
}

void CashDataProvider::logBoughtItem(int32_t userid, int32_t charid, int32_t packageid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO cashshop_sell_log VALUES (NULL, " << userid << ", " << charid << ", " << packageid << ")";
	query.exec();
}
