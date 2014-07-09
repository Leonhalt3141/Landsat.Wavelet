import scala.math._

object fwt_test {
  val L = 64
  val L2 = 32
  val N = 64
  val N2 = 32
  val NN = 4096
  val K = 4
  val PI = 3.14159265358979
  
  def main(args: Array[String]): Unit = {

    var s0 = new Array[Double](L);
    var s0r = new Array[Double](L);
    
    var s1 = new Array[Double](L2);
    var w1 = new Array[Double](L2);
    
    val p: Array[Double] = Array(0.482962913145, 0.836516303738, 
    							0.224143868042, -0.129409522551);
    val q = new Array[Double](K);
    
    val sup = K
    val s_len = L
    
    for(i <- 0 to 16-1){
      s0(i) = ((i+1) * (i+1)).toDouble / 256.0;
    }
    for(i <- 16 to 32-1){
      s0(i) = 0.2;
    }
    for(i <- 32 to 48-1){
      s0(i) = ((47-i)*(47-i)) / 256.0 - 0.5;
    }
    
    for(i <- 0 to sup-1){
      q(i) = pow(-1.0, (i)) * p(sup-i-1);
    }
    
    println("Input Signal")
    for(i <- 0 to L-1){
      printf("%7.4f  ", s0(i))
    }
    print("\n")
    
    val fwtmap: Map[String, Array[Double]] = fwt1d(s0, p, q, sup, s1, w1)
    s1 = fwtmap("s1")
    w1 = fwtmap("w1")
    
    s0r = ifwt1d(s1, w1, p, q, sup, s0r)
    
    println("Reconstractred Signal")
    for(i <-  0 to L-1){
      printf("%7.4f  ", s0r(i))
    }
  }

  def fwt1d(s0: Array[Double], p: Array[Double], 
      q: Array[Double], sup: Int, s1: Array[Double], w1: Array[Double]): Map[String, Array[Double]] = {
    for(k <- 0 to L/2-1){
      s1(k) = 0.0
      w1(k) = 0.0
      for(n <- 0 to sup-1){
        val index = (n + 2 * k) % L;
        s1(k) += p(n) * s0(index)
        w1(k) += q(n) * s0(index)
      }
    }
    val result: Map[String, Array[Double]] = Map("s1" -> s1, "w1" -> w1)
    return result
  }
  
  def ifwt1d(s1: Array[Double], w1: Array[Double], p: Array[Double],
      q: Array[Double], sup: Int, s0r: Array[Double]): Array[Double] = {
    val ofs = max(1024, L)
    //var s0: Array[Double] = null;
    
    for(n <- 0 to (L/2)-1){
      s0r(2*n+1) = 0.0
      s0r(2*n) = 0.0
      for(k <- 0 to (sup/2)-1){
        val index = (n - k + ofs) % (L/2);
        s0r((2*n)+1) += p((2*k)+1) * s1(index) + q((2*k)+1) * w1(index);
        s0r(2*n) += p(2*k) * s1(index) + q(2*k) * w1(index);
      }
    }
    return s0r;
  }

  
}