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
	echo "\$rpa instanceof ReflectionPropertyAccessor = ".($rpa instanceof ReflectionPropertyAccessor).PHP_EOL;
	echo "\$rpa instanceof ReflectionProperty = ".($rpa instanceof ReflectionProperty).PHP_EOL;
?>
==DONE==
--EXPECT--
Class of $rpa = $rc->getProperty('a1') is ReflectionPropertyAccessor
$rpa instanceof ReflectionPropertyAccessor = 1
$rpa instanceof ReflectionProperty = 1
==DONE==
