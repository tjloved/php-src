--TEST--
Testing callback in alias
--FILE--
<?php

class foo
{
    public static function test()
    {
        print "hello\n";
    }
    public function test2()
    {
        print "foobar!\n";
    }
}
function fn361913007()
{
    class_alias('FOO', 'bar');
    call_user_func(array('bar', 'test'));
    $a = new bar();
    call_user_func(array($a, 'test2'));
}
fn361913007();
--EXPECT--
hello
foobar!
