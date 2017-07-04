--TEST--
instanceof shouldn't call __autoload
--FILE--
<?php

class A
{
}
function fn341925297()
{
    spl_autoload_register(function ($name) {
        echo "AUTOLOAD '{$name}'\n";
        eval("class {$name} {}");
    });
    $a = new A();
    var_dump($a instanceof B);
    var_dump($a instanceof A);
}
fn341925297();
--EXPECT--
bool(false)
bool(true)
