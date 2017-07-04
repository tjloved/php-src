--TEST--
Bug #72177 Scope issue in __destruct after ReflectionProperty::setValue()
--FILE--
<?php

class Foo
{
    private $bar = 'bar';
    public function __construct()
    {
        unset($this->bar);
    }
}
class Bar extends Foo
{
    private $baz = 'baz';
    private static $tab = 'tab';
    public function __get(string $name)
    {
        var_dump($this->baz);
        var_dump(self::$tab);
        return $name;
    }
}
function fn386004149()
{
    $r = new ReflectionProperty(Foo::class, 'bar');
    $r->setAccessible(true);
    echo "OK\n";
}
fn386004149();
--EXPECT--
OK
