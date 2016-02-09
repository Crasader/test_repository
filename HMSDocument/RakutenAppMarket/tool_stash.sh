#!/bin/sh


#----------------------------------------------------------------------------------
# 線内部の変数は使用する環境に応じて適切な変更を行って下さい

# stashされたファイルが移動するルートフォルダ
backup_path="./backup"

# android projectのルート
android_project_root="../../../war/proj.android"

# google関係の退避させるフォルダ(android projectとの相対パス)
google_stash_files=("src/org/cocos2dx/cpp/AppActivity.java")
google_stash_files+=("src/org/cocos2dx/cpp/GooglePlayInAppBilling.java")
google_stash_files+=("src/org/cocos2dx/cpp/PurchaseData.java")
google_stash_files+=("src/jp/pkga/war/gcm/GcmBroadcastReceiver.java")
google_stash_files+=("src/jp/pkga/war/gcm/GcmIntentService.java")
google_stash_files+=("src/jp/pkga/war/gcm/GcmManager.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/Base64.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/Base64DecoderException.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/IabException.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/IabHelper.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/IabResult.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/Inventory.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/Purchase.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/Security.java")
google_stash_files+=("src/jp/pkga/war/inappbilling/util/google/SkuDetails.java")
google_stash_files+=("src/com/android/vending/billing/IInAppBillingService.aidl")

# 楽天関係の退避させるフォルダ(android projectとの相対パス)
rakuten_stash_files=("src/org/cocos2dx/rakutencpp/AppActivityForRakuten.java")
rakuten_stash_files+=("src/org/cocos2dx/rakutencpp/InAppBillingForRakuten.java")
rakuten_stash_files+=("src/org/cocos2dx/rakutencpp/PurchaseDataForRakuten.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/Base64.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/Base64DecoderException.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/IabException.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/IabHelper.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/IabResult.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/Inventory.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/Purchase.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/Security.java")
rakuten_stash_files+=("src/jp/pkga/war/inappbilling/util/rakuten/SkuDetails.java")
rakuten_stash_files+=("src/jp/co/rakuten/appmarket/billing/IInRakutenCheckoutService.aidl")

#----------------------------------------------------------------------------------

echo "stash:不要なファイルを避難させる"
echo "restore:避難したファイルを元の場所に戻す"
echo "stashされたファイルは${backup_path}下にプロジェクトと同階層で作成"

PS3='select number -->'
select target in rakuten_stash rakuten_restore google_stash google_restore  exit; do

	case $target in
		rakuten_stash)
			stash_files=("${rakuten_stash_files[@]}")
			arrow="stash"
		;;

		rakuten_restore)
			stash_files=("${rakuten_stash_files[@]}")
			arrow="restore"
		;;

		google_stash)
			stash_files=("${google_stash_files[@]}")
			arrow="stash"
		;;

		google_restore)
			stash_files=("${google_stash_files[@]}")
			arrow="restore"
		;;

		exit)
			break ;
		;;

		*)
			break ;
		;;
	esac

	for file in "${stash_files[@]}"; do
		stash_dir="${file%/*}" 
		mkdir -p "$backup_path/$stash_dir" 
		if [ $arrow == "stash" ]; then
			mv -v "$android_project_root/$file" "$backup_path/$stash_dir"
		elif [ $arrow == "restore" ]; then
			mv -v "$backup_path/$file" "$android_project_root/$stash_dir" 
		fi
	done

done

