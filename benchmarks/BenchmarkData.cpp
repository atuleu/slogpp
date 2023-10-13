#include "BenchmarkData.hpp"

#include <random>
#include <type_traits>

static std::mt19937_64 gen{};

template <size_t N, std::enable_if_t<N >= 1> * = nullptr>
static std::uniform_int_distribution<size_t> index_distribution{0, N - 1};

template <typename Integer, Integer Min, Integer Max>
static std::uniform_int_distribution<Integer> integer_distribution{Min, Max};

template <typename Floating, int Min, int Max>
static std::uniform_real_distribution<Floating> float_distribution{Min, Max};

template <typename T, size_t N> const T &choose(T (&array)[N]) {
	return array[index_distribution<N>(gen)];
}

template <int64_t value = 60000000000> Timepoint random_time_in_past() {
	return Timepoint::clock::now() +
	       Duration(integer_distribution<int64_t, -value, 0>(gen));
}

BenchmarkData::Request::Request() {
	static std::string roots[] = {
	    "http://www.example.com",    "https://www.w3.org",
	    "https://www.wikipedia.org", "http://www.github.com",
	    "https://www.cnn.com",       "https://www.amazon.com",
	    "http://www.nytimes.com",    "https://www.stackoverflow.com",
	    "http://www.microsoft.com",  "https://www.apple.com",
	    "http://www.google.com",     "https://www.yahoo.com",
	    "http://www.craigslist.org", "https://www.reddit.com",
	    "http://www.linkedin.com",   "https://www.netflix.com",
	    "http://www.instagram.com",  "https://www.facebook.com",
	    "http://www.twitter.com",    "https://www.youtube.com"};
	static std::string commonURIs[] = {
	    "/",          "/home",    "/about",    "/contact", "/products",
	    "/services",  "/blog",    "/articles", "/news",    "/portfolio",
	    "/faq",       "/login",   "/register", "/profile", "/settings",
	    "/downloads", "/support", "/events",   "/gallery", "/sitemap"};

	static int statuses[] = {
	    100, 101, 102, 103, 122, // Informational responses
	    200, 201, 202, 203, 204, 205, 206, 207, 208, 226, // Success
	    300, 301, 302, 303, 304, 305, 306, 307, 308,      // Redirection
	    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410,
	    411, 412, 413, 414, 415, 416, 417, 418, 421, 422, 423,
	    424, 425, 426, 428, 429, 431, 451,                    // Client errors
	    500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511 // Server errors
	};

	status = choose(statuses);
	url    = choose(roots) + choose(commonURIs);
}

BenchmarkData::BenchmarkData()
    : code{integer_distribution<int, -20000, 0>(gen)}
    , value{float_distribution<double, -100, 100>(gen)}
    , duration{integer_distribution<int64_t, -60000000000, 60000000000>(gen)}
    , time{random_time_in_past()} {

	static std::string animals[] = {
	    "Armadillo", "Bear",    "Cheetah", "Dolphin",     "Elephant",
	    "Fox",       "Giraffe", "Horse",   "Iguana",      "Jaguar",
	    "Kangaroo",  "Lion",    "Monkey",  "Nightingale", "Ostrich",
	    "Penguin",   "Quokka",  "Raccoon", "Sloth",       "Tiger",
	    "Uakari",    "Vulture", "Walrus",  "X-ray Tetra", "Yak",
	    "Zebra"};

	static std::string adjectives[] = {
	    "Amiable",       "Brilliant", "Caring",    "Diligent",  "Energetic",
	    "Friendly",      "Gracious",  "Hopeful",   "Inventive", "Joyful",
	    "Kind-hearted",  "Loyal",     "Modest",    "Nurturing", "Optimistic",
	    "Patient",       "Quaint",    "Resilient", "Sincere",   "Thoughtful",
	    "Understanding", "Vivacious", "Witty",     "Xenial",    "Youthful",
	    "Zesty"};

	domain = choose(adjectives) + " " + choose(animals);
}
