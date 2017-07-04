--TEST--
Testing variable variables as function name
--FILE--
<?php

class bar
{
    public function a()
    {
        return "bar!";
    }
}
class foo
{
    public function test()
    {
        print "foo!\n";
        return new bar();
    }
}
function test()
{
    return new foo();
}
function fn1735578889()
{
    $a = 'ucfirst';
    $b = 'a';
    print ${$b}('test');
    print "\n";
    $a = 'test';
    $b = 'a';
    var_dump(${$b}()->{${$b}}()->{$b}());
    $a = 'strtoupper';
    $b = 'a';
    $c = 'b';
    $d = 'c';
    var_dump(${${${$d}}}('foo'));
}
fn1735578889();
--EXPECT--
Test
foo!
string(4) "bar!"
string(3) "FOO"
