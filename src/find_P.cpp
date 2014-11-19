#include <cpd/find_P.hpp>

#include <figtree.h>


namespace cpd
{


double find_P(
        const arma::mat& X,
        const arma::mat& Y,
        double sigma2,
        double outliers,
        arma::vec& P1,
        arma::vec& Pt1,
        arma::mat& PX,
        bool use_fgt
        )
{
    P1.zeros();
    Pt1.zeros();
    PX.zeros();

    const arma::uword N = X.n_rows;
    const arma::uword M = Y.n_rows;
    const arma::uword D = Y.n_cols;

    const int eval_method = use_fgt ? FIGTREE_EVAL_AUTO : FIGTREE_EVAL_DIRECT;

    arma::vec denomP(N);

    const double h = std::sqrt(2 * sigma2);
    const double ndi = (outliers * M * std::pow(2 * M_PI * sigma2, 0.5 * D)) / ((1 - outliers) * N);
    const int W = 1;
    const double epsilon = 1e-2;
    arma::vec q = arma::ones<arma::vec>(M);

    arma::mat Xt = X.t();
    arma::mat Yt = Y.t();

    figtree(D, M, N, W, Yt.memptr(), h, q.memptr(), Xt.memptr(), epsilon, denomP.memptr(), eval_method);

    denomP = denomP + ndi;
    Pt1 = 1 - ndi / denomP;
    q = 1 / denomP;

    figtree(D, N, M, W, Xt.memptr(), h, q.memptr(), Yt.memptr(), epsilon, P1.memptr(), eval_method);

    for (int i = 0; i < D; ++i)
    {
        q = X.col(i) / denomP;
        figtree(D, N, M, W, Xt.memptr(), h, q.memptr(), Yt.memptr(), epsilon, PX.colptr(i), eval_method);
    }

    return -arma::sum(arma::log(denomP)) + D * N * std::log(sigma2) / 2;
}


}
