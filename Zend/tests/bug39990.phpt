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
function fn1168112416()
{
    $obj = new Foo();
    foreach ($obj->arr as $value) {
        echo "{$value}\n";
    }
}
fn1168112416();
--EXPECT--
Hello
World
