--TEST--
IntlCalendar::equals(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

var_dump($c->equals());

try { var_dump($c->equals(new stdclass)); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }
try { var_dump($c->equals(1, 2)); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }

try { var_dump(intlcal_equals($c, array())); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }
try { var_dump(intlcal_equals(1, $c)); }
catch (Exception $e) { echo get_class($e), ': ', $e->getMessage(), "\n"; }

--EXPECT--
error: 2, IntlCalendar::equals() expects exactly 1 parameter, 0 given
error: 2, IntlCalendar::equals(): intlcal_equals: bad arguments
bool(false)
EngineException: Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, instance of stdClass given
EngineException: Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, integer given
EngineException: Argument 2 passed to intlcal_equals() must be an instance of IntlCalendar, array given
EngineException: Argument 1 passed to intlcal_equals() must be an instance of IntlCalendar, integer given
