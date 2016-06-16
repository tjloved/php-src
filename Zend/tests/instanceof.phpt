--TEST--
instanceof shouldn't call __autoload
--FILE--
<?php

function __autoload($name)
{
    echo "AUTOLOAD '{$name}'\n";
    eval("class {$name} {}");
}
class A
{
}
function fn2050240871()
{
    $a = new A();
    var_dump($a instanceof B);
    var_dump($a instanceof A);
}
fn2050240871();
--EXPECT--
bool(false)
bool(true)
