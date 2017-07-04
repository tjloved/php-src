--TEST--
Try catch finally catch(multi catch blocks)
--FILE--
<?php

class AE extends Exception
{
}
class BE extends Exception
{
}
function foo()
{
    try {
        try {
            try {
                try {
                    echo "1";
                    throw new Exception("try");
                } catch (AE $e) {
                    die("error");
                } finally {
                    echo "2";
                }
            } finally {
                echo "3";
            }
        } catch (BE $e) {
            die("error");
        } finally {
            echo "4";
        }
    } catch (Exception $e) {
        echo "5";
    } catch (AE $e) {
        die("error");
    } finally {
        echo "6";
    }
    return 7;
}
function fn724455123()
{
    var_dump(foo());
}
fn724455123();
--EXPECTF--
123456int(7)
