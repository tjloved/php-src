--TEST--
ZE2 Visibility of property is ignored when involving an accessor (accessor visibility checked)
--FILE--
<?php

class Foo {
    protected $bar {
        public get { return 'test'; }
    }
}

$foo = new Foo;
var_dump($foo->bar);

?>
===DONE===
--EXPECTF--
string(%d) "test"
===DONE===