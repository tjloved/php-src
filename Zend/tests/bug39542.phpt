--TEST--
Bug #39542 (Behaviour of require_once/include_once different to < 5.2.0)
--FILE--
<?php

function __autoload($class)
{
    if (!(require_once $class . '.php')) {
        error_log('Error: Autoload class: ' . $class . ' not found!');
    }
}
function fn677482097()
{
    $oldcwd = getcwd();
    chdir(dirname(__FILE__));
    if (substr(PHP_OS, 0, 3) == 'WIN') {
        set_include_path(dirname(__FILE__) . '/bug39542;.');
    } else {
        set_include_path(dirname(__FILE__) . '/bug39542:.');
    }
    new bug39542();
    chdir($oldcwd);
}
fn677482097();
--EXPECT--
ok
