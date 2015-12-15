--TEST--
Bug #70089 (segfault in PHP 7 at ZEND_FETCH_DIM_W_SPEC_VAR_CONST_HANDLER ())
--INI--
opcache.enable=0
--FILE--
<?php
function dummy($a) {
}

try {
	chr(0)[0][] = 1;
} catch (Error $e) {
	var_dump($e->getMessage());
}
try {
	unset(chr(0)[0][0]);
} catch (Error $e) {
	var_dump($e->getMessage());
}
eval("function runtimetest(&\$a) {} ");
try {
	runtimetest(chr(0)[0]);
} catch (Error $e) {
	var_dump($e->getMessage());
}

try {
	++chr(0)[0];
} catch (Error $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "Cannot indirectly modify string offset"
string(%d) "Cannot indirectly modify string offset"
string(%d) "Cannot indirectly modify string offset"
string(%d) "Cannot indirectly modify string offset"
