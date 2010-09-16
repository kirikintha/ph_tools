<?php
/**
 * @name Custom Enhancements
 * A set of re-useable functions:
 * @static _strip_views_keys($str)
 * @static _set_header_nocache()
 * @static _browser_check()
 * @static _list_months()
 * @static _list_years($startDate, $endDate)
 * @static _list_canadian_provinces()
 * @static _list_us_states()
 * @static _list_countries_iso()
 * @static _list_countries_iso_cck()
 * @static _get_season($to = 0)
 */

class phTools {
  
  /**
   * @name _get_body_classes()
   * There are a lot of conflisting classes added to the body of a Drupal page, this allows you to eliminate them
   * @param string $vars['body_classes'] a string of system classes added from Drupal
   * @return string a string of body classes to add to the <body> of the page
   */
  public static function _get_body_classes($vars) {
    $inputs = explode(' ', $vars['body_classes']); // Default classes
    $bodyClasses = array();
    //Add in the node-% before anything else, so that the node admin overrides anything else when we apply classes
    //Node Add Class
    if (arg(0) == 'node' && arg(1) == 'add') {
      $bodyClasses['node-add'] = 'node-add';
    }
    
    //Node Edit Class
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'edit') {
      $bodyClasses['node-edit'] = 'node-edit';
    }
    //Node Edit Class when editing node revisions
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'revisions' && is_numeric(arg(3)) && arg(4) == 'edit') {
      $bodyClasses['node-edit'] = 'node-edit';
    }
     //turn the body classes into a keyed array so we can unset them if we want them.
    foreach ($inputs as $input) {
      $bodyClasses[$input] = check_plain($input);
    }
    return implode(' ', $bodyClasses); // Create class list separated by spaces
  }

	/**
   * @name _strip_views_keys($str)
   * Strips out repetetive views keys from views array keys, helpful when trying to cut down the lengths of very long views array keys
   * @internal - marked for deletion, but this is sometimes nice to have.
   */
	public static function _strip_views_keys($str) {
		$keywords = explode(";", variable_get('ph_tools_views_strip_keys_keys', ''));
		$mixedSearch 	= array();
		$mixedReplace = array();
		foreach ($keywords as $key => $value) {
			$mixedSearch[] 	= $value;
			$mixedReplace[] = '';
		}
		$str = str_replace($mixedSearch, $mixedReplace, $str);
		return $str;
	}
	
  /**
   * @name _set_header_nocache()
   * Set browser not to cache AJAX return results.
   */
	public static function _set_header_nocache() {
		header("Expires: Sun, 19 Nov 1978 05:00:00 GMT");
		header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
		header("Cache-Control: no-store, no-cache, must-revalidate");
		header("Cache-Control: post-check=0, pre-check=0", false);
		header("Pragma: no-cache");
	}
	
	/**
   * @name _browser_check()
   * @return an array $browser with the keys: $browser['version'] && $browser['type']
   */
	public static function _browser_check() {
		$useragent = $_SERVER['HTTP_USER_AGENT'];
		if (preg_match('|MSIE ([0-9].[0-9]{1,2})|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'IE';
		} 
		elseif (preg_match('|Opera ([0-9].[0-9]{1,2})|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Opera';
		} 
		elseif(preg_match('|Firefox/([0-9\.]+)|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Firefox';
		} 
		elseif(preg_match('|Safari/([0-9\.]+)|',$useragent,$matched)) {
			$browser['version'] =$matched[1];
			$browser['type'] = 'Safari';
		} 
		else {
		// browser not recognized!
			$browser['version']  = 0;
			$browser['type']= 'other';
		}
		return $browser;
	}
	
	/**
   * @name _list_months()
   * @return array of all months listed in the Julian Calendar
   */
	public static function _list_months() {
		return array(
			'01' 	=> 'January (01)',
			'02' 	=> 'February (02)',
			'03' 	=> 'March (03)',
			'04' 	=> 'April (04)',
			'05' 	=> 'May (05)',
			'06' 	=> 'June (06)',
			'07' 	=> 'July (07)',
			'08' 	=> 'August (08)',
			'09' 	=> 'September (09)',
			'10' 	=> 'October (10)',
			'11' 	=> 'November (11)',
			'12' 	=> 'December (12)',
		);
	}
		
	/**
   * @name _list_years($startDate, $endDate)
	 * List all Years in the form of yyyy in between the start date and end date you supply.
	 * Start Date must be a date earlier than the end date
	 * @param $startDate
	 * @param $endDate
	 * @return array $years[YYYY] = YYYY
	 */
	public static function _list_years($startDate, $endDate) {
		$years = array();
		$count = $endDate - $startDate;
		$i = 0;
		if ($startDate < $endDate) {
			while ($i <= $count) {
				$newYear = $startDate + $i;
				$years[$newYear] = $newYear;
				$i++;
			}
			return $years;
		} else {
			return;
		}
	}
	
	/**
	 * @name _list_canadian_provinces()
	 * @return array a list of Canadian Provinces that CCK can understand
	 */
	public static function _list_canadian_provinces() {
		return array(
			'AB'=>'Alberta',
			'BC'=>'British Columbia',
			'LB'=>'Labrador',
			'MB'=>'Manitoba',
			'NB'=>'New Brunswick',
			'NL'=>'Newfoundland',
			'NT'=>'Northwest Territories',
			'NS'=>'Nova Scotia',
			'NU'=>'Nunavut',
			'ON'=>'Ontario',
			'PE'=>'Prince Edward Island',
			'QC'=>'Quebec',
			'SK'=>'Saskatchewan',
			'YT'=>'Yukon'
		);
	}
	
	/**
	 * @name _list_us_states()
	 * @return array a list of US states that CCK fields can easily understand
	 */
	public static function _list_us_states() {
		return array(
			'AL'=>'Alabama',
			'AK'=>'Alaska',
			'AZ'=>'Arizona',
			'AR'=>'Arkansas',
			'CA'=>'California',
			'CO'=>'Colorado',
			'CT'=>'Connecticut',
			'DE'=>'Delaware',
			'DC'=>'District Of Columbia',
			'FL'=>'Florida',
			'GA'=>'Georgia',
			'HI'=>'Hawaii',
			'ID'=>'Idaho',
			'IL'=>'Illinois',
			'IN'=>'Indiana',
			'IA'=>'Iowa',
			'KS'=>'Kansas',
			'KY'=>'Kentucky',
			'LA'=>'Louisiana',
			'ME'=>'Maine',
			'MD'=>'Maryland',
			'MA'=>'Massachusetts',
			'MI'=>'Michigan',
			'MN'=>'Minnesota',
			'MS'=>'Mississippi',
			'MO'=>'Missouri',
			'MT'=>'Montana',
			'NE'=>'Nebraska',
			'NV'=>'Nevada',
			'NH'=>'New Hampshire',
			'NJ'=>'New Jersey',
			'NM'=>'New Mexico',
			'NY'=>'New York',
			'NC'=>'North Carolina',
			'ND'=>'North Dakota',
			'OH'=>'Ohio',
			'OK'=>'Oklahoma',
			'OR'=>'Oregon',
			'PA'=>'Pennsylvania',
			'RI'=>'Rhode Island',
			'SC'=>'South Carolina',
			'SD'=>'South Dakota',
			'TN'=>'Tennessee',
			'TX'=>'Texas',
			'UT'=>'Utah',
			'VT'=>'Vermont',
			'VA'=>'Virginia',
			'WA'=>'Washington',
			'WV'=>'West Virginia',
			'WI'=>'Wisconsin',
			'WY'=>'Wyoming',
    );
	}
	
  /**
   * @name _list_countries_iso()
   * A list of current world countries and their ISO code, useful when using shipping
   * @return array of countries, with the keys: upper lower alpha3 numeric
   */
	public static function _list_countries_iso() {
		return array(
			'AF' => array('upper'=>'AFGHANISTAN','lower'=>'Afghanistan','alpha3'=>'AFG','numeric'=>'004'),
			'AL' => array('upper'=>'ALBANIA','lower'=>'Albania','alpha3'=>'ALB','numeric'=>'008'),
			'DZ' => array('upper'=>'ALGERIA','lower'=>'Algeria','alpha3'=>'DZA','numeric'=>'012'),
			'AS' => array('upper'=>'AMERICAN SAMOA','lower'=>'American Samoa','alpha3'=>'ASM','numeric'=>'016'),
			'AD' => array('upper'=>'ANDORRA','lower'=>'Andorra','alpha3'=>'AND','numeric'=>'020'),
			'AO' => array('upper'=>'ANGOLA','lower'=>'Angola','alpha3'=>'AGO','numeric'=>'024'),
			'AI' => array('upper'=>'ANGUILLA','lower'=>'Anguilla','alpha3'=>'AIA','numeric'=>'660'),
			'AQ' => array('upper'=>'ANTARCTICA','lower'=>'Antarctica','alpha3'=>NULL,'numeric'=>NULL),
			'AG' => array('upper'=>'ANTIGUA AND BARBUDA','lower'=>'Antigua and Barbuda','alpha3'=>'ATG','numeric'=>'028'),
			'AR' => array('upper'=>'ARGENTINA','lower'=>'Argentina','alpha3'=>'ARG','numeric'=>'032'),
			'AM' => array('upper'=>'ARMENIA','lower'=>'Armenia','alpha3'=>'ARM','numeric'=>'051'),
			'AW' => array('upper'=>'ARUBA','lower'=>'Aruba','alpha3'=>'ABW','numeric'=>'533'),
			'AU' => array('upper'=>'AUSTRALIA','lower'=>'Australia','alpha3'=>'AUS','numeric'=>'036'),
			'AT' => array('upper'=>'AUSTRIA','lower'=>'Austria','alpha3'=>'AUT','numeric'=>'040'),
			'AZ' => array('upper'=>'AZERBAIJAN','lower'=>'Azerbaijan','alpha3'=>'AZE','numeric'=>'031'),
			'BS' => array('upper'=>'BAHAMAS','lower'=>'Bahamas','alpha3'=>'BHS','numeric'=>'044'),
			'BH' => array('upper'=>'BAHRAIN','lower'=>'Bahrain','alpha3'=>'BHR','numeric'=>'048'),
			'BD' => array('upper'=>'BANGLADESH','lower'=>'Bangladesh','alpha3'=>'BGD','numeric'=>'050'),
			'BB' => array('upper'=>'BARBADOS','lower'=>'Barbados','alpha3'=>'BRB','numeric'=>'052'),
			'BY' => array('upper'=>'BELARUS','lower'=>'Belarus','alpha3'=>'BLR','numeric'=>'112'),
			'BE' => array('upper'=>'BELGIUM','lower'=>'Belgium','alpha3'=>'BEL','numeric'=>'056'),
			'BZ' => array('upper'=>'BELIZE','lower'=>'Belize','alpha3'=>'BLZ','numeric'=>'084'),
			'BJ' => array('upper'=>'BENIN','lower'=>'Benin','alpha3'=>'BEN','numeric'=>'204'),
			'BM' => array('upper'=>'BERMUDA','lower'=>'Bermuda','alpha3'=>'BMU','numeric'=>'060'),
			'BT' => array('upper'=>'BHUTAN','lower'=>'Bhutan','alpha3'=>'BTN','numeric'=>'064'),
			'BO' => array('upper'=>'BOLIVIA','lower'=>'Bolivia','alpha3'=>'BOL','numeric'=>'068'),
			'BA' => array('upper'=>'BOSNIA AND HERZEGOVINA','lower'=>'Bosnia and Herzegovina','alpha3'=>'BIH','numeric'=>'070'),
			'BW' => array('upper'=>'BOTSWANA','lower'=>'Botswana','alpha3'=>'BWA','numeric'=>'072'),
			'BV' => array('upper'=>'BOUVET ISLAND','lower'=>'Bouvet Island','alpha3'=>NULL,'numeric'=>NULL),
			'BR' => array('upper'=>'BRAZIL','lower'=>'Brazil','alpha3'=>'BRA','numeric'=>'076'),
			'IO' => array('upper'=>'BRITISH INDIAN OCEAN TERRITORY','lower'=>'British Indian Ocean Territory','alpha3'=>NULL,'numeric'=>NULL),
			'BN' => array('upper'=>'BRUNEI DARUSSALAM','lower'=>'Brunei Darussalam','alpha3'=>'BRN','numeric'=>'096'),
			'BG' => array('upper'=>'BULGARIA','lower'=>'Bulgaria','alpha3'=>'BGR','numeric'=>'100'),
			'BF' => array('upper'=>'BURKINA FASO','lower'=>'Burkina Faso','alpha3'=>'BFA','numeric'=>'854'),
			'BI' => array('upper'=>'BURUNDI','lower'=>'Burundi','alpha3'=>'BDI','numeric'=>'108'),
			'KH' => array('upper'=>'CAMBODIA','lower'=>'Cambodia','alpha3'=>'KHM','numeric'=>'116'),
			'CM' => array('upper'=>'CAMEROON','lower'=>'Cameroon','alpha3'=>'CMR','numeric'=>'120'),
			'CA' => array('upper'=>'CANADA','lower'=>'Canada','alpha3'=>'CAN','numeric'=>'124'),
			'CV' => array('upper'=>'CAPE VERDE','lower'=>'Cape Verde','alpha3'=>'CPV','numeric'=>'132'),
			'KY' => array('upper'=>'CAYMAN ISLANDS','lower'=>'Cayman Islands','alpha3'=>'CYM','numeric'=>'136'),
			'CF' => array('upper'=>'CENTRAL AFRICAN REPUBLIC','lower'=>'Central African Republic','alpha3'=>'CAF','numeric'=>'140'),
			'TD' => array('upper'=>'CHAD','lower'=>'Chad','alpha3'=>'TCD','numeric'=>'148'),
			'CL' => array('upper'=>'CHILE','lower'=>'Chile','alpha3'=>'CHL','numeric'=>'152'),
			'CN' => array('upper'=>'CHINA','lower'=>'China','alpha3'=>'CHN','numeric'=>'156'),
			'CX' => array('upper'=>'CHRISTMAS ISLAND','lower'=>'Christmas Island','alpha3'=>NULL,'numeric'=>NULL),
			'CC' => array('upper'=>'COCOSKEELING ISLANDS','lower'=>'CocosKeeling Islands','alpha3'=>NULL,'numeric'=>NULL),
			'CO' => array('upper'=>'COLOMBIA','lower'=>'Colombia','alpha3'=>'COL','numeric'=>'170'),
			'KM' => array('upper'=>'COMOROS','lower'=>'Comoros','alpha3'=>'COM','numeric'=>'174'),
			'CG' => array('upper'=>'CONGO','lower'=>'Congo','alpha3'=>'COG','numeric'=>'178'),
			'CD' => array('upper'=>'CONGO, THE DEMOCRATIC REPUBLIC OF THE','lower'=>'Congo, the Democratic Republic of the','alpha3'=>'COD','numeric'=>'180'),
			'CK' => array('upper'=>'COOK ISLANDS','lower'=>'Cook Islands','alpha3'=>'COK','numeric'=>'184'),
			'CR' => array('upper'=>'COSTA RICA','lower'=>'Costa Rica','alpha3'=>'CRI','numeric'=>'188'),
			'CI' => array('upper'=>'COTE D\'IVOIRE','lower'=>'Cote D\'Ivoire','alpha3'=>'CIV','numeric'=>'384'),
			'HR' => array('upper'=>'CROATIA','lower'=>'Croatia','alpha3'=>'HRV','numeric'=>'191'),
			'CU' => array('upper'=>'CUBA','lower'=>'Cuba','alpha3'=>'CUB','numeric'=>'192'),
			'CY' => array('upper'=>'CYPRUS','lower'=>'Cyprus','alpha3'=>'CYP','numeric'=>'196'),
			'CZ' => array('upper'=>'CZECH REPUBLIC','lower'=>'Czech Republic','alpha3'=>'CZE','numeric'=>'203'),
			'DK' => array('upper'=>'DENMARK','lower'=>'Denmark','alpha3'=>'DNK','numeric'=>'208'),
			'DJ' => array('upper'=>'DJIBOUTI','lower'=>'Djibouti','alpha3'=>'DJI','numeric'=>'262'),
			'DM' => array('upper'=>'DOMINICA','lower'=>'Dominica','alpha3'=>'DMA','numeric'=>'212'),
			'DO' => array('upper'=>'DOMINICAN REPUBLIC','lower'=>'Dominican Republic','alpha3'=>'DOM','numeric'=>'214'),
			'EC' => array('upper'=>'ECUADOR','lower'=>'Ecuador','alpha3'=>'ECU','numeric'=>'218'),
			'EG' => array('upper'=>'EGYPT','lower'=>'Egypt','alpha3'=>'EGY','numeric'=>'818'),
			'SV' => array('upper'=>'EL SALVADOR','lower'=>'El Salvador','alpha3'=>'SLV','numeric'=>'222'),
			'GQ' => array('upper'=>'EQUATORIAL GUINEA','lower'=>'Equatorial Guinea','alpha3'=>'GNQ','numeric'=>'226'),
			'ER' => array('upper'=>'ERITREA','lower'=>'Eritrea','alpha3'=>'ERI','numeric'=>'232'),
			'EE' => array('upper'=>'ESTONIA','lower'=>'Estonia','alpha3'=>'EST','numeric'=>'233'),
			'ET' => array('upper'=>'ETHIOPIA','lower'=>'Ethiopia','alpha3'=>'ETH','numeric'=>'231'),
			'FK' => array('upper'=>'FALKLAND ISLANDSMALVINAS','lower'=>'Falkland IslandsMalvinas','alpha3'=>'FLK','numeric'=>'238'),
			'FO' => array('upper'=>'FAROE ISLANDS','lower'=>'Faroe Islands','alpha3'=>'FRO','numeric'=>'234'),
			'FJ' => array('upper'=>'FIJI','lower'=>'Fiji','alpha3'=>'FJI','numeric'=>'242'),
			'FI' => array('upper'=>'FINLAND','lower'=>'Finland','alpha3'=>'FIN','numeric'=>'246'),
			'FR' => array('upper'=>'FRANCE','lower'=>'France','alpha3'=>'FRA','numeric'=>'250'),
			'GF' => array('upper'=>'FRENCH GUIANA','lower'=>'French Guiana','alpha3'=>'GUF','numeric'=>'254'),
			'PF' => array('upper'=>'FRENCH POLYNESIA','lower'=>'French Polynesia','alpha3'=>'PYF','numeric'=>'258'),
			'TF' => array('upper'=>'FRENCH SOUTHERN TERRITORIES','lower'=>'French Southern Territories','alpha3'=>NULL,'numeric'=>NULL),
			'GA' => array('upper'=>'GABON','lower'=>'Gabon','alpha3'=>'GAB','numeric'=>'266'),
			'GM' => array('upper'=>'GAMBIA','lower'=>'Gambia','alpha3'=>'GMB','numeric'=>'270'),
			'GE' => array('upper'=>'GEORGIA','lower'=>'Georgia','alpha3'=>'GEO','numeric'=>'268'),
			'DE' => array('upper'=>'GERMANY','lower'=>'Germany','alpha3'=>'DEU','numeric'=>'276'),
			'GH' => array('upper'=>'GHANA','lower'=>'Ghana','alpha3'=>'GHA','numeric'=>'288'),
			'GI' => array('upper'=>'GIBRALTAR','lower'=>'Gibraltar','alpha3'=>'GIB','numeric'=>'292'),
			'GR' => array('upper'=>'GREECE','lower'=>'Greece','alpha3'=>'GRC','numeric'=>'300'),
			'GL' => array('upper'=>'GREENLAND','lower'=>'Greenland','alpha3'=>'GRL','numeric'=>'304'),
			'GD' => array('upper'=>'GRENADA','lower'=>'Grenada','alpha3'=>'GRD','numeric'=>'308'),
			'GP' => array('upper'=>'GUADELOUPE','lower'=>'Guadeloupe','alpha3'=>'GLP','numeric'=>'312'),
			'GU' => array('upper'=>'GUAM','lower'=>'Guam','alpha3'=>'GUM','numeric'=>'316'),
			'GT' => array('upper'=>'GUATEMALA','lower'=>'Guatemala','alpha3'=>'GTM','numeric'=>'320'),
			'GN' => array('upper'=>'GUINEA','lower'=>'Guinea','alpha3'=>'GIN','numeric'=>'324'),
			'GW' => array('upper'=>'GUINEA-BISSAU','lower'=>'Guinea-Bissau','alpha3'=>'GNB','numeric'=>'624'),
			'GY' => array('upper'=>'GUYANA','lower'=>'Guyana','alpha3'=>'GUY','numeric'=>'328'),
			'HT' => array('upper'=>'HAITI','lower'=>'Haiti','alpha3'=>'HTI','numeric'=>'332'),
			'HM' => array('upper'=>'HEARD ISLAND AND MCDONALD ISLANDS','lower'=>'Heard Island and Mcdonald Islands','alpha3'=>NULL,'numeric'=>NULL),
			'VA' => array('upper'=>'HOLY SEE (VATICAN CITY STATE)','lower'=>'Holy See (Vatican City State)','alpha3'=>'VAT','numeric'=>'336'),
			'HN' => array('upper'=>'HONDURAS','lower'=>'Honduras','alpha3'=>'HND','numeric'=>'340'),
			'HK' => array('upper'=>'HONG KONG','lower'=>'Hong Kong','alpha3'=>'HKG','numeric'=>'344'),
			'HU' => array('upper'=>'HUNGARY','lower'=>'Hungary','alpha3'=>'HUN','numeric'=>'348'),
			'IS' => array('upper'=>'ICELAND','lower'=>'Iceland','alpha3'=>'ISL','numeric'=>'352'),
			'IN' => array('upper'=>'INDIA','lower'=>'India','alpha3'=>'IND','numeric'=>'356'),
			'ID' => array('upper'=>'INDONESIA','lower'=>'Indonesia','alpha3'=>'IDN','numeric'=>'360'),
			'IR' => array('upper'=>'IRAN, ISLAMIC REPUBLIC OF','lower'=>'Iran, Islamic Republic of','alpha3'=>'IRN','numeric'=>'364'),
			'IQ' => array('upper'=>'IRAQ','lower'=>'Iraq','alpha3'=>'IRQ','numeric'=>'368'),
			'IE' => array('upper'=>'IRELAND','lower'=>'Ireland','alpha3'=>'IRL','numeric'=>'372'),
			'IL' => array('upper'=>'ISRAEL','lower'=>'Israel','alpha3'=>'ISR','numeric'=>'376'),
			'IT' => array('upper'=>'ITALY','lower'=>'Italy','alpha3'=>'ITA','numeric'=>'380'),
			'JM' => array('upper'=>'JAMAICA','lower'=>'Jamaica','alpha3'=>'JAM','numeric'=>'388'),
			'JP' => array('upper'=>'JAPAN','lower'=>'Japan','alpha3'=>'JPN','numeric'=>'392'),
			'JO' => array('upper'=>'JORDAN','lower'=>'Jordan','alpha3'=>'JOR','numeric'=>'400'),
			'KZ' => array('upper'=>'KAZAKHSTAN','lower'=>'Kazakhstan','alpha3'=>'KAZ','numeric'=>'398'),
			'KE' => array('upper'=>'KENYA','lower'=>'Kenya','alpha3'=>'KEN','numeric'=>'404'),
			'KI' => array('upper'=>'KIRIBATI','lower'=>'Kiribati','alpha3'=>'KIR','numeric'=>'296'),
			'KP' => array('upper'=>'KOREA', 'lower'=>'Korea, Democratic People\'s Republic of','alpha3'=>'PRK','numeric'=>'408'),
			'KR' => array('upper'=>'KOREA', 'lower'=>'Korea, Republic of','alpha3'=>'KOR','numeric'=>'410'),
			'KW' => array('upper'=>'KUWAIT','lower'=>'Kuwait','alpha3'=>'KWT','numeric'=>'414'),
			'KG' => array('upper'=>'KYRGYZSTAN','lower'=>'Kyrgyzstan','alpha3'=>'KGZ','numeric'=>'417'),
			'LA' => array('upper'=>'LAO PEOPLE\'S DEMOCRATIC REPUBLIC','lower'=>'Lao People\'s Democratic Republic','alpha3'=>'LAO','numeric'=>'418'),
			'LV' => array('upper'=>'LATVIA','lower'=>'Latvia','alpha3'=>'LVA','numeric'=>'428'),
			'LB' => array('upper'=>'LEBANON','lower'=>'Lebanon','alpha3'=>'LBN','numeric'=>'422'),
			'LS' => array('upper'=>'LESOTHO','lower'=>'Lesotho','alpha3'=>'LSO','numeric'=>'426'),
			'LR' => array('upper'=>'LIBERIA','lower'=>'Liberia','alpha3'=>'LBR','numeric'=>'430'),
			'LY' => array('upper'=>'LIBYAN ARAB JAMAHIRIYA','lower'=>'Libyan Arab Jamahiriya','alpha3'=>'LBY','numeric'=>'434'),
			'LI' => array('upper'=>'LIECHTENSTEIN','lower'=>'Liechtenstein','alpha3'=>'LIE','numeric'=>'438'),
			'LT' => array('upper'=>'LITHUANIA','lower'=>'Lithuania','alpha3'=>'LTU','numeric'=>'440'),
			'LU' => array('upper'=>'LUXEMBOURG','lower'=>'Luxembourg','alpha3'=>'LUX','numeric'=>'442'),
			'MO' => array('upper'=>'MACAO','lower'=>'Macao','alpha3'=>'MAC','numeric'=>'446'),
			'MK' => array('upper'=>'MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF','lower'=>'Macedonia, the Former Yugoslav Republic of','alpha3'=>'MKD','numeric'=>'807'),
			'MG' => array('upper'=>'MADAGASCAR','lower'=>'Madagascar','alpha3'=>'MDG','numeric'=>'450'),
			'MW' => array('upper'=>'MALAWI','lower'=>'Malawi','alpha3'=>'MWI','numeric'=>'454'),
			'MY' => array('upper'=>'MALAYSIA','lower'=>'Malaysia','alpha3'=>'MYS','numeric'=>'458'),
			'MV' => array('upper'=>'MALDIVES','lower'=>'Maldives','alpha3'=>'MDV','numeric'=>'462'),
			'ML' => array('upper'=>'MALI','lower'=>'Mali','alpha3'=>'MLI','numeric'=>'466'),
			'MT' => array('upper'=>'MALTA','lower'=>'Malta','alpha3'=>'MLT','numeric'=>'470'),
			'MH' => array('upper'=>'MARSHALL ISLANDS','lower'=>'Marshall Islands','alpha3'=>'MHL','numeric'=>'584'),
			'MQ' => array('upper'=>'MARTINIQUE','lower'=>'Martinique','alpha3'=>'MTQ','numeric'=>'474'),
			'MR' => array('upper'=>'MAURITANIA','lower'=>'Mauritania','alpha3'=>'MRT','numeric'=>'478'),
			'MU' => array('upper'=>'MAURITIUS','lower'=>'Mauritius','alpha3'=>'MUS','numeric'=>'480'),
			'YT' => array('upper'=>'MAYOTTE','lower'=>'Mayotte','alpha3'=>NULL,'numeric'=>NULL),
			'MX' => array('upper'=>'MEXICO','lower'=>'Mexico','alpha3'=>'MEX','numeric'=>'484'),
			'FM' => array('upper'=>'MICRONESIA, FEDERATED STATES OF','lower'=>'Micronesia, Federated States of','alpha3'=>'FSM','numeric'=>'583'),
			'MD' => array('upper'=>'MOLDOVA, REPUBLIC OF','lower'=>'Moldova, Republic of','alpha3'=>'MDA','numeric'=>'498'),
			'MC' => array('upper'=>'MONACO','lower'=>'Monaco','alpha3'=>'MCO','numeric'=>'492'),
			'MN' => array('upper'=>'MONGOLIA','lower'=>'Mongolia','alpha3'=>'MNG','numeric'=>'496'),
			'MS' => array('upper'=>'MONTSERRAT','lower'=>'Montserrat','alpha3'=>'MSR','numeric'=>'500'),
			'MA' => array('upper'=>'MOROCCO','lower'=>'Morocco','alpha3'=>'MAR','numeric'=>'504'),
			'MZ' => array('upper'=>'MOZAMBIQUE','lower'=>'Mozambique','alpha3'=>'MOZ','numeric'=>'508'),
			'MM' => array('upper'=>'MYANMAR','lower'=>'Myanmar','alpha3'=>'MMR','numeric'=>'104'),
			'NA' => array('upper'=>'NAMIBIA','lower'=>'Namibia','alpha3'=>'NAM','numeric'=>'516'),
			'NR' => array('upper'=>'NAURU','lower'=>'Nauru','alpha3'=>'NRU','numeric'=>'520'),
			'NP' => array('upper'=>'NEPAL','lower'=>'Nepal','alpha3'=>'NPL','numeric'=>'524'),
			'NL' => array('upper'=>'NETHERLANDS','lower'=>'Netherlands','alpha3'=>'NLD','numeric'=>'528'),
			'AN' => array('upper'=>'NETHERLANDS ANTILLES','lower'=>'Netherlands Antilles','alpha3'=>'ANT','numeric'=>'530'),
			'NC' => array('upper'=>'NEW CALEDONIA','lower'=>'New Caledonia','alpha3'=>'NCL','numeric'=>'540'),
			'NZ' => array('upper'=>'NEW ZEALAND','lower'=>'New Zealand','alpha3'=>'NZL','numeric'=>'554'),
			'NI' => array('upper'=>'NICARAGUA','lower'=>'Nicaragua','alpha3'=>'NIC','numeric'=>'558'),
			'NE' => array('upper'=>'NIGER','lower'=>'Niger','alpha3'=>'NER','numeric'=>'562'),
			'NG' => array('upper'=>'NIGERIA','lower'=>'Nigeria','alpha3'=>'NGA','numeric'=>'566'),
			'NU' => array('upper'=>'NIUE','lower'=>'Niue','alpha3'=>'NIU','numeric'=>'570'),
			'NF' => array('upper'=>'NORFOLK ISLAND','lower'=>'Norfolk Island','alpha3'=>'NFK','numeric'=>'574'),
			'MP' => array('upper'=>'NORTHERN MARIANA ISLANDS','lower'=>'Northern Mariana Islands','alpha3'=>'MNP','numeric'=>'580'),
			'NO' => array('upper'=>'NORWAY','lower'=>'Norway','alpha3'=>'NOR','numeric'=>'578'),
			'OM' => array('upper'=>'OMAN','lower'=>'Oman','alpha3'=>'OMN','numeric'=>'512'),
			'PK' => array('upper'=>'PAKISTAN','lower'=>'Pakistan','alpha3'=>'PAK','numeric'=>'586'),
			'PW' => array('upper'=>'PALAU','lower'=>'Palau','alpha3'=>'PLW','numeric'=>'585'),
			'PS' => array('upper'=>'PALESTINIAN TERRITORY, OCCUPIED','lower'=>'Palestinian Territory, Occupied','alpha3'=>NULL,'numeric'=>NULL),
			'PA' => array('upper'=>'PANAMA','lower'=>'Panama','alpha3'=>'PAN','numeric'=>'591'),
			'PG' => array('upper'=>'PAPUA NEW GUINEA','lower'=>'Papua New Guinea','alpha3'=>'PNG','numeric'=>'598'),
			'PY' => array('upper'=>'PARAGUAY','lower'=>'Paraguay','alpha3'=>'PRY','numeric'=>'600'),
			'PE' => array('upper'=>'PERU','lower'=>'Peru','alpha3'=>'PER','numeric'=>'604'),
			'PH' => array('upper'=>'PHILIPPINES','lower'=>'Philippines','alpha3'=>'PHL','numeric'=>'608'),
			'PN' => array('upper'=>'PITCAIRN','lower'=>'Pitcairn','alpha3'=>'PCN','numeric'=>'612'),
			'PL' => array('upper'=>'POLAND','lower'=>'Poland','alpha3'=>'POL','numeric'=>'616'),
			'PT' => array('upper'=>'PORTUGAL','lower'=>'Portugal','alpha3'=>'PRT','numeric'=>'620'),
			'PR' => array('upper'=>'PUERTO RICO','lower'=>'Puerto Rico','alpha3'=>'PRI','numeric'=>'630'),
			'QA' => array('upper'=>'QATAR','lower'=>'Qatar','alpha3'=>'QAT','numeric'=>'634'),
			'RE' => array('upper'=>'REUNION','lower'=>'Reunion','alpha3'=>'REU','numeric'=>'638'),
			'RO' => array('upper'=>'ROMANIA','lower'=>'Romania','alpha3'=>'ROM','numeric'=>'642'),
			'RU' => array('upper'=>'RUSSIAN FEDERATION','lower'=>'Russian Federation','alpha3'=>'RUS','numeric'=>'643'),
			'RW' => array('upper'=>'RWANDA','lower'=>'Rwanda','alpha3'=>'RWA','numeric'=>'646'),
			'SH' => array('upper'=>'SAINT HELENA','lower'=>'Saint Helena','alpha3'=>'SHN','numeric'=>'654'),
			'KN' => array('upper'=>'SAINT KITTS AND NEVIS','lower'=>'Saint Kitts and Nevis','alpha3'=>'KNA','numeric'=>'659'),
			'LC' => array('upper'=>'SAINT LUCIA','lower'=>'Saint Lucia','alpha3'=>'LCA','numeric'=>'662'),
			'PM' => array('upper'=>'SAINT PIERRE AND MIQUELON','lower'=>'Saint Pierre and Miquelon','alpha3'=>'SPM','numeric'=>'666'),
			'VC' => array('upper'=>'SAINT VINCENT AND THE GRENADINES','lower'=>'Saint Vincent and the Grenadines','alpha3'=>'VCT','numeric'=>'670'),
			'WS' => array('upper'=>'SAMOA','lower'=>'Samoa','alpha3'=>'WSM','numeric'=>'882'),
			'SM' => array('upper'=>'SAN MARINO','lower'=>'San Marino','alpha3'=>'SMR','numeric'=>'674'),
			'ST' => array('upper'=>'SAO TOME AND PRINCIPE','lower'=>'Sao Tome and Principe','alpha3'=>'STP','numeric'=>'678'),
			'SA' => array('upper'=>'SAUDI ARABIA','lower'=>'Saudi Arabia','alpha3'=>'SAU','numeric'=>'682'),
			'SN' => array('upper'=>'SENEGAL','lower'=>'Senegal','alpha3'=>'SEN','numeric'=>'686'),
			'CS' => array('upper'=>'SERBIA AND MONTENEGRO','lower'=>'Serbia and Montenegro','alpha3'=>NULL,'numeric'=>NULL),
			'SC' => array('upper'=>'SEYCHELLES','lower'=>'Seychelles','alpha3'=>'SYC','numeric'=>'690'),
			'SL' => array('upper'=>'SIERRA LEONE','lower'=>'Sierra Leone','alpha3'=>'SLE','numeric'=>'694'),
			'SG' => array('upper'=>'SINGAPORE','lower'=>'Singapore','alpha3'=>'SGP','numeric'=>'702'),
			'SK' => array('upper'=>'SLOVAKIA','lower'=>'Slovakia','alpha3'=>'SVK','numeric'=>'703'),
			'SI' => array('upper'=>'SLOVENIA','lower'=>'Slovenia','alpha3'=>'SVN','numeric'=>'705'),
			'SB' => array('upper'=>'SOLOMON ISLANDS','lower'=>'Solomon Islands','alpha3'=>'SLB','numeric'=>'090'),
			'SO' => array('upper'=>'SOMALIA','lower'=>'Somalia','alpha3'=>'SOM','numeric'=>'706'),
			'ZA' => array('upper'=>'SOUTH AFRICA','lower'=>'South Africa','alpha3'=>'ZAF','numeric'=>'710'),
			'GS' => array('upper'=>'SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS','lower'=>'South Georgia and the South Sandwich Islands','alpha3'=>NULL,'numeric'=>NULL),
			'ES' => array('upper'=>'SPAIN','lower'=>'Spain','alpha3'=>'ESP','numeric'=>'724'),
			'LK' => array('upper'=>'SRI LANKA','lower'=>'Sri Lanka','alpha3'=>'LKA','numeric'=>'144'),
			'SD' => array('upper'=>'SUDAN','lower'=>'Sudan','alpha3'=>'SDN','numeric'=>'736'),
			'SR' => array('upper'=>'SURINAME','lower'=>'Suriname','alpha3'=>'SUR','numeric'=>'740'),
			'SJ' => array('upper'=>'SVALBARD AND JAN MAYEN','lower'=>'Svalbard and Jan Mayen','alpha3'=>'SJM','numeric'=>'744'),
			'SZ' => array('upper'=>'SWAZILAND','lower'=>'Swaziland','alpha3'=>'SWZ','numeric'=>'748'),
			'SE' => array('upper'=>'SWEDEN','lower'=>'Sweden','alpha3'=>'SWE','numeric'=>'752'),
			'CH' => array('upper'=>'SWITZERLAND','lower'=>'Switzerland','alpha3'=>'CHE','numeric'=>'756'),
			'SY' => array('upper'=>'SYRIAN ARAB REPUBLIC','lower'=>'Syrian Arab Republic','alpha3'=>'SYR','numeric'=>'760'),
			'TW' => array('upper'=>'TAIWAN, PROVINCE OF CHINA','lower'=>'Taiwan, Province of China','alpha3'=>'TWN','numeric'=>'158'),
			'TJ' => array('upper'=>'TAJIKISTAN','lower'=>'Tajikistan','alpha3'=>'TJK','numeric'=>'762'),
			'TZ' => array('upper'=>'TANZANIA, UNITED REPUBLIC OF','lower'=>'Tanzania, United Republic of','alpha3'=>'TZA','numeric'=>'834'),
			'TH' => array('upper'=>'THAILAND','lower'=>'Thailand','alpha3'=>'THA','numeric'=>'764'),
			'TL' => array('upper'=>'TIMOR-LESTE','lower'=>'Timor-Leste','alpha3'=>NULL,'numeric'=>NULL),
			'TG' => array('upper'=>'TOGO','lower'=>'Togo','alpha3'=>'TGO','numeric'=>'768'),
			'TK' => array('upper'=>'TOKELAU','lower'=>'Tokelau','alpha3'=>'TKL','numeric'=>'772'),
			'TO' => array('upper'=>'TONGA','lower'=>'Tonga','alpha3'=>'TON','numeric'=>'776'),
			'TT' => array('upper'=>'TRINIDAD AND TOBAGO','lower'=>'Trinidad and Tobago','alpha3'=>'TTO','numeric'=>'780'),
			'TN' => array('upper'=>'TUNISIA','lower'=>'Tunisia','alpha3'=>'TUN','numeric'=>'788'),
			'TR' => array('upper'=>'TURKEY','lower'=>'Turkey','alpha3'=>'TUR','numeric'=>'792'),
			'TM' => array('upper'=>'TURKMENISTAN','lower'=>'Turkmenistan','alpha3'=>'TKM','numeric'=>'795'),
			'TC' => array('upper'=>'TURKS AND CAICOS ISLANDS','lower'=>'Turks and Caicos Islands','alpha3'=>'TCA','numeric'=>'796'),
			'TV' => array('upper'=>'TUVALU','lower'=>'Tuvalu','alpha3'=>'TUV','numeric'=>'798'),
			'UG' => array('upper'=>'UGANDA','lower'=>'Uganda','alpha3'=>'UGA','numeric'=>'800'),
			'UA' => array('upper'=>'UKRAINE','lower'=>'Ukraine','alpha3'=>'UKR','numeric'=>'804'),
			'AE' => array('upper'=>'UNITED ARAB EMIRATES','lower'=>'United Arab Emirates','alpha3'=>'ARE','numeric'=>'784'),
			'GB' => array('upper'=>'UNITED KINGDOM','lower'=>'United Kingdom','alpha3'=>'GBR','numeric'=>'826'),
			'US' => array('upper'=>'UNITED STATES','lower'=>'United States','alpha3'=>'USA','numeric'=>'840'),
			'UM' => array('upper'=>'UNITED STATES MINOR OUTLYING ISLANDS','lower'=>'United States Minor Outlying Islands','alpha3'=>NULL,'numeric'=>NULL),
			'UY' => array('upper'=>'URUGUAY','lower'=>'Uruguay','alpha3'=>'URY','numeric'=>'858'),
			'UZ' => array('upper'=>'UZBEKISTAN','lower'=>'Uzbekistan','alpha3'=>'UZB','numeric'=>'860'),
			'VU' => array('upper'=>'VANUATU','lower'=>'Vanuatu','alpha3'=>'VUT','numeric'=>'548'),
			'VE' => array('upper'=>'VENEZUELA','lower'=>'Venezuela','alpha3'=>'VEN','numeric'=>'862'),
			'VN' => array('upper'=>'VIET NAM','lower'=>'Viet Nam','alpha3'=>'VNM','numeric'=>'704'),
			'VG' => array('upper'=>'VIRGIN ISLANDS, BRITISH','lower'=>'Virgin Islands, British','alpha3'=>'VGB','numeric'=>'092'),
			'VI' => array('upper'=>'VIRGIN ISLANDS, U.S.','lower'=>'Virgin Islands, U.s.','alpha3'=>'VIR','numeric'=>'850'),
			'WF' => array('upper'=>'WALLIS AND FUTUNA','lower'=>'Wallis and Futuna','alpha3'=>'WLF','numeric'=>'876'),
			'EH' => array('upper'=>'WESTERN SAHARA','lower'=>'Western Sahara','alpha3'=>'ESH','numeric'=>'732'),
			'YE' => array('upper'=>'YEMEN','lower'=>'Yemen','alpha3'=>'YEM','numeric'=>'887'),
			'ZM' => array('upper'=>'ZAMBIA','lower'=>'Zambia','alpha3'=>'ZMB','numeric'=>'894'),
			'ZW' => array('upper'=>'ZIMBABWE','lower'=>'Zimbabwe','alpha3'=>'ZWE','numeric'=>'716')
		);
	}
  
  /**
   * @name _list_countries_iso_cck()
   * @return array a list of countries that CCK fields can easily understand
   */
  public static function _list_countries_iso_cck() {
    $countries  = phTools::_list_countries_iso();
    $countryArr = array('NULL' => '- None -');
    foreach ($countries as $key => $value) {
      $countryArr[$key] = $value['lower'];
    }
    return $countryArr;
  }
  
	/**
   * @name _get_season($to = 0)
   * Get the season name, Winter, Autumn, Summer, Spring, Winter and it's date
   * @param integer $to a value in the future for the season you wish to select
   * @return array $season['name'] $season['date']
   */
	public static function _get_season($to = 0) {
		if (is_numeric($to)) {
			$seasonDates = array('/12/21'=>'Winter', '/09/21'=>'Autumn', '/06/21'=>'Summer', '/03/21'=>'Spring', '/01/01'=>'Winter');
			$season = array();
			foreach ($seasonDates as $key => $value) {
				$seasonDate = date("Y", strtotime("now + $to days")) .$key;
				if (strtotime("now + $to days") > strtotime($seasonDate)) {
					$season['name'] = $value;
					$season['date'] = $key;
					return $season;
				}
			}
		} else {
			drupal_set_message('You must use a numeric value for the variable $to _getSeason($to), please look at this public static function and enter in a numeric value.', 'error');
		}
	}
  
  /**
   * @name _detectMobileDevice($ignoreAgents)
   * Detect if a user agent is a mobile device or not. This uses the theory that we are not looking for desktop browsers, rather than looking for all mobile devices
   * based on: http://smartmobtoolkit.wordpress.com/2008/10/16/not-device-detection
   * @param array $ignoreUserAgents = an array of values to ignore in a user agent to ignore, such as the ipad
   * @return bool true/false if a device is a mibile device or not
   */
  public static function _detectMobileDevice($ignoreAgents = array('ipad')) {
    //Operating Systems we want to ignore
    $os       = array('Windows', 'Macintosh', 'Linux', 'FreeBSD', 'Solaris');
    $crawlers = array('bot', 'slurp', 'spider', 'crawl');
    $ignore   = array_merge($os, $crawlers);
    //Turn the ignore array into a piped preg_match query
    $ignore = implode('|', $ignore); 
    //If we match anything on the ignore list, and do not have mobile in the user agent string, return false
    if (preg_match('/('.$ignore.')/i', $_SERVER['HTTP_USER_AGENT'], $matches) && !strstr(strtoupper($_SERVER['HTTP_USER_AGENT']), 'MOBILE')) {
      return false;
    } else {
      //If we do not match on the ignore list then we are presuming we are a mobile device, unless that mobile device is to be ignored
      $ignoreAgents = implode('|', $ignoreAgents);
      if (preg_match('/('.$ignoreAgents.')/i', $_SERVER['HTTP_USER_AGENT'], $matches)) {
        return false;
      } else {
        return true;
      } 
    }
  }

}

/**
 * @name phToolsWidgets
 */

class phToolsWidgets {
  
  /**
    * @name _get_weather_widget($url)
    * returns a weather widget from NOAA's XML
    * @param string $url a valid url for creating the widget
    * @return html of the weather widget from the $widget array
    * $widget->icon_url_base
    * $widget->icon_url_name
    * $widget->weather
    * $widget->temperature_string
    * $widget->wind_string
    * $widget->relative_humidity
    * $widget->dewpoint_string
    * $widget->visibility_mi
    * $widget->windchill_string
    * $widget->observation_time
    * $widget->credit
  */
  public static function _get_weather_widget($url) {
    
    if (!empty($url) && strstr($url, 'http://www.weather.gov/xml/current_obs')) { //check to see if the url is valid
      
      $url = $url . '?nocache=' .md5(time()); //make sure the xml does not cache
      $xml = simplexml_load_file ($url);
      
      //print_r($xml);
      
      $widget = new stdClass;
      
      foreach ($xml as $key => $value) {
        
        $widget->$key = (string) trim($value);
        
      }
      
      return theme('weather_widget', $widget);
      
    } else {
      
      drupal_set_message(t('You have not entered in a valid url, please enter in a url destination from http://www.weather.gov/xml/current_obs'), 'error');
      
    }
    
  }
  
}

/**
 * @name _ceAdmin
 */
class phToolsAdmin {
  
  /**
   * @name _checkLibraries
   * Check the path you have given for jquery libraries
   */
  public static function _checkLibrary() {
    $files = file_scan_directory(variable_get('ph_tools_library_directory', NULL), variable_get('ph_tools_library_whitelist', PH_TOOLS_MODULE_WHITELIST), $nomask = array('.', '..', 'jquery.js', 'CVS', 'fckeditor', 'tinymce'));
    $form  = array();
    if (!empty($files)) {
      //If we find jquery libraries here, then allow them as settings parameters. We want to ignore directories with fck or tiny because those are reserved for the wysiwyg module.
      foreach ($files as $key => $file) {
        $options = array(
         0               => 'DISABLE',
         $file->filename => 'ENABLE',
       );
        $form['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)] = array(
          '#title' 				 => t('Enable or Disable @name', array('@name' => $file->name)),
          '#description' 	 => t('This file is located at: @dir', array('@dir' => $file->filename)),
          '#type' 				 => 'radios', 
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name), 0),
          '#options' 			 => $options,
          '#weight'        => 0,
          '#group'         => 'jquery_plugins',
          '#prefix'        => '<hr />',
       );
        //Allows us to set the scope if we are enabled
        $scope = array('header' => t('Header'), 'footer' => t('Footer'));
        $form['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_scope'] = array(
          '#title' 				 => t('Scope'),
          '#description' 	 => t('Set the Scope for this script for the header or the footer of the page.'),
          '#type' 				 => 'radios', 
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_scope', 'header'),
          '#options' 			 => $scope,
          '#weight'        => 9,
          '#group'         => 'jquery_plugins',
       );
        $form['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name)]['ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_paths'] = array(
          '#title' 				 => t('Paths to exclude this script from.'),
          '#description' 	 => t('Set the pages in which you do not wish to have this script run.'),
          '#type' 				 => 'textarea', 
          '#default_value' => variable_get('ph_tools_enable_' .str_replace(array('.','-'),array('_','_'),$file->name) .'_paths', variable_get('ph_tools_global_paths', "admin*\n")),
          '#weight'        => 10,
          '#group'         => 'jquery_plugins',
       );
      }
    }
    return $form;
  }
  
  /**
   * @name _checkPath();
   * Checks for TRUE/FALSE if we can instantiate a jquery plug-in
   * @return bool true/false
   */
  public static function _check_path($paths = NULL) {
    if ($paths != NULL) {
      $path 	= drupal_get_path_alias($_GET['q']);
      $regexp = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1'. preg_quote(variable_get('site_frontpage', 'node'), '/') .'\2'), preg_quote($paths, '/')) .')$/';
      $found 	= preg_match($regexp, $path);
      if ($found == 1) {
        return FALSE;
      }
    }
    //Else we have a match, then we can invoke our caller.
    return TRUE;
  }
  
  /**
   * @name _ph_tools_load_js()
   * Load Jquery, Jquery UI and Preload images via the admin settings
   */
  public static function _ph_tools_load_js() {
    //Global settings, this is for the main settings of the module.
    if (self::_check_path(variable_get('ph_tools_global_paths',''))) {
      //Add Destination Setting, so you can have the drupal_get_destination() as a js available Drupal.settings.
      drupal_add_js(array('drupal_get_destination' => drupal_get_destination()), 'setting');
      //Add Path To Theme setting, so you can get the path to theme.
      $current_theme  = variable_get('theme_default','none');
      $themes         = list_themes();
      $theme_object   = $themes[$current_theme];
      drupal_add_js(array('drupal_get_path_to_theme' => drupal_get_path('theme', $theme_object->name)), 'setting');
      //Extended Drupal JS functions, under the Drupal.PHtools.%function
      drupal_add_js(PH_TOOLS_MODULE_PATH. '/js/phtools.drupal.functions.js','module','header');
      //Add Fade Functionality.
      self::_message_can_fade();
    }
    //Loop through each library setting and add the file information if we have a valid page.
    $settings = variable_get('ph_tools_library', array());
    if (!empty($settings)) {
      foreach ($settings as $key => $setting) {
        if (self::_check_path($setting['paths'])) {
          drupal_add_js($setting['script'], 'module', $setting['scope']);
        }
      }
    }
  }
  
  /**
   * @name _message_can_fade()
   * Find out if the path given is valid for fading out messagess.
   */
  public static function _message_can_fade() {
    //Check to see if the message can fade, then rock it out.
    if (variable_get('phtools_fade_messages_override', 0) == 1) {
      //Settings for fade out delay.
      $settings['ph_tools_fade_out_delay'] = variable_get('ph_tools_fade_out_delay',5500);
      //Add these to Drupal.settings
      drupal_add_js($settings, 'setting');
      drupal_add_js('$(document).ready(function() { Drupal.PHTools.canFade(); });', 'inline', 'footer');
    }
      
  }

}