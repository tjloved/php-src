--TEST--
ReflectionPropertyAccessor is_a(ReflectionProperty)
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		public $a1 { get; set; }
	}
	
	$rc = new ReflectionClass('TimePeriod');
	$rpa = $rc->getProperty('a1');
	
	echo "Class of \$rpa = \$rc->getProperty('a1') is ".get_class($rpa).PHP_EOL;
	echo "\$rpa is_a(ReflectionPropertyAccessor) = ".(int)is_a($rpa, 'ReflectionPropertyAccessor').PHP_EOL;
	echo "\$rpa is_a(ReflectionProperty) = ".(int)is_a($rpa, 'ReflectionProperty').PHP_EOL;
?>
==DONE==
--EXPECT--
Class of $rpa = $rc->getProperty('a1') is ReflectionPropertyAccessor
$rpa is_a(ReflectionPropertyAccessor) = 1
$rpa is_a(ReflectionProperty) = 1
==DONE==
