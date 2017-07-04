--TEST--
Bug #51822 (Segfault with strange __destruct() for static class variables)
--FILE--
<?php

class DestructableObject
{
    public function __destruct()
    {
        echo "2\n";
    }
}
class DestructorCreator
{
    public function __destruct()
    {
        $this->test = new DestructableObject();
        echo "1\n";
    }
}
class Test
{
    public static $mystatic;
}
function fn1995450981()
{
    // Uncomment this to avoid segfault
    //Test::$mystatic = new DestructorCreator();
    $x = new Test();
    if (!isset(Test::$mystatic)) {
        Test::$mystatic = new DestructorCreator();
    }
    echo "bla\n";
}
fn1995450981();
--EXPECT--
bla
1
2
