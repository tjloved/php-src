--TEST--
Bug #63468 (wrong called method as callback with inheritance)
--FILE--
<?php

class Foo
{
    public function run()
    {
        return call_user_func(array('Bar', 'getValue'));
    }
    private static function getValue()
    {
        return 'Foo';
    }
}
class Bar extends Foo
{
    public static function getValue()
    {
        return 'Bar';
    }
}
function fn166083906()
{
    $x = new Bar();
    var_dump($x->run());
}
fn166083906();
--EXPECT--
string(3) "Bar"

