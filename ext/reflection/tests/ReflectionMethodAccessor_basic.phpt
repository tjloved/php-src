--TEST--
Tests ReflectionMethodAccessor gets returned properly and isAutoImplemented() works
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		public $a1 {
			get { return 1; }
			set;
		}
	}
	$rc = new ReflectionClass('TimePeriod');
	foreach($rc->getProperties() as $objProperty) {
		echo "\$objGetter = \$objProperty->getGet();".PHP_EOL;
		$objGetter = $objProperty->getGet();
		var_dump($objGetter);
		
		echo "\$objGetter instanceof ReflectionMethodAccessor = ".($objGetter instanceof ReflectionMethodAccessor).PHP_EOL;
		echo "\$objGetter instanceof ReflectionMethod = ".($objGetter instanceof ReflectionMethod).PHP_EOL;
		echo "\$objGetter->isPublic() = ".var_export($objGetter->isPublic(), true).PHP_EOL;
		echo "\$objGetter->isAutoImplemented() = ".var_export($objGetter->isAutoImplemented(), true).PHP_EOL;
		echo PHP_EOL;

		echo "\$objSetter = \$objProperty->getSet();".PHP_EOL;
		$objSetter = $objProperty->getSet();
		var_dump($objSetter);
		echo "\$objSetter->isAutoImplemented() = ".var_export($objSetter->isAutoImplemented(), true).PHP_EOL;
	}
?>
==DONE==
--EXPECT--
$objGetter = $objProperty->getGet();
object(ReflectionMethodAccessor)#3 (2) {
  ["name"]=>
  string(8) "$a1->get"
  ["class"]=>
  string(10) "TimePeriod"
}
$objGetter instanceof ReflectionMethodAccessor = 1
$objGetter instanceof ReflectionMethod = 1
$objGetter->isPublic() = true
$objGetter->isAutoImplemented() = false

$objSetter = $objProperty->getSet();
object(ReflectionMethodAccessor)#4 (2) {
  ["name"]=>
  string(8) "$a1->set"
  ["class"]=>
  string(10) "TimePeriod"
}
$objSetter->isAutoImplemented() = true
==DONE==
