--TEST--
Bug #33999 (object remains object when cast to int)
--INI--
error_reporting=4095
--FILE--
<?php

class Foo
{
    public $bar = "bat";
}
function fn1740007270()
{
    $foo = new Foo();
    var_dump($foo);
    $bar = (int) $foo;
    var_dump($bar);
    $baz = (double) $foo;
    var_dump($baz);
}
fn1740007270();
--EXPECTF--
object(Foo)#1 (1) {
  ["bar"]=>
  string(3) "bat"
}

Notice: Object of class Foo could not be converted to int in %sbug33999.php on line %d
int(1)

Notice: Object of class Foo could not be converted to float in %sbug33999.php on line %d
float(1)
