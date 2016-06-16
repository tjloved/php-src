--TEST--
Bug #39990 (Cannot "foreach" over overloaded properties)
--FILE--
<?php

class Foo
{
    public function __get($name)
    {
        return array('Hello', 'World');
    }
}
function fn1713071525()
{
    $obj = new Foo();
    foreach ($obj->arr as $value) {
        echo "{$value}\n";
    }
}
fn1713071525();
--EXPECT--
Hello
World
