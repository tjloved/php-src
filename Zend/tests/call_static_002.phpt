--TEST--
Testing __call and __callstatic with callbacks
--FILE--
<?php

class Foo
{
    public function __call($a, $b)
    {
        print "nonstatic\n";
        var_dump($a);
    }
    public static function __callStatic($a, $b)
    {
        print "static\n";
        var_dump($a);
    }
}
function fn754540114()
{
    $a = new Foo();
    call_user_func(array($a, 'aAa'));
    call_user_func(array('Foo', 'aAa'));
}
fn754540114();
--EXPECTF--
nonstatic
string(3) "aAa"
static
string(3) "aAa"
