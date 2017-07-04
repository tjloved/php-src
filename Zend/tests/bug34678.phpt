--TEST--
Bug #34678 (__call(), is_callable() and static methods)
--FILE--
<?php

class A
{
    public function __call($m, $a)
    {
        echo "__call\n";
    }
}
class B extends A
{
    public static function foo()
    {
        echo "foo\n";
    }
}
function fn131782303()
{
    if (is_callable(array('B', 'foo'))) {
        call_user_func(array('B', 'foo'));
    }
    if (is_callable(array('A', 'foo'))) {
        call_user_func(array('A', 'foo'));
    }
}
fn131782303();
--EXPECT--
foo
