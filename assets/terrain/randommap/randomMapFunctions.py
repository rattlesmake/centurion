def NormalizeData(data):
    return (data - np.min(data)) / (np.max(data) - np.min(data))

def fftIndgen(n):
    a = list(range(0, int(n/2+1)))
    b = list(range(1, int(n/2)))
    b.reverse()
    b = [-i for i in b]
    return a + b

def gaussian_random_field(Pk, size_x, size_y):
    def Pk2(kx, ky):
        if kx == 0 and ky == 0:
            return 0.0
        return np.sqrt(Pk(np.sqrt(kx**2 + ky**2)))
    noise = np.fft.fft2(np.random.normal(size = (size_x, size_y)))
    amplitude = np.zeros((size_x,size_y))
    for i, kx in enumerate(fftIndgen(size_x)):
        for j, ky in enumerate(fftIndgen(size_y)):            
            amplitude[i, j] = Pk2(kx, ky)
    return np.fft.ifft2(noise * amplitude).real

def get_heightmap(seed, sizeX, sizeY):
    np.random.seed(seed)
    alpha = -3.75
    M = gaussian_random_field(Pk = lambda k: k**alpha, size_x = sizeX, size_y = sizeY)
    return NormalizeData(np.asarray(M.T).reshape(-1)) * 2.75