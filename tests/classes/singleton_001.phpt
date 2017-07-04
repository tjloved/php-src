--TEST--
ZE2 singleton
--FILE--
<?php

class Counter
{
    private $counter = 0;
    function increment_and_print()
    {
        echo ++$this->counter;
        echo "\n";
    }
}
class SingletonCounter
{
    private static $m_instance = NULL;
    static function Instance()
    {
        if (self::$m_instance == NULL) {
            self::$m_instance = new Counter();
        }
        return self::$m_instance;
    }
}
function fn1053949527()
{
    SingletonCounter::Instance()->increment_and_print();
    SingletonCounter::Instance()->increment_and_print();
    SingletonCounter::Instance()->increment_and_print();
}
fn1053949527();
--EXPECT--
1
2
3
