--TEST--
ZE2 Tests that an undefined setter produces an error on unset()
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
	}
}

$o = new AccessorTest();

unset($o->b);
?>
--EXPECTF--
Fatal error: Cannot set property AccessorTest::$b, no setter defined. in %s on line %d

This test needs to be re-worked / checked against v1.2 changes.  Technically it is working as it should
(unset is not causing an error) but that "feature" has not yet been put in place.  
Need to trace what's going on here.